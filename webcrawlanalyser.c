#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

void load_and_merge_json(const char *files[], int num_files, const char *output_file) {
    cJSON *merged_data = cJSON_CreateArray();
    
    for (int i = 0; i < num_files; i++) {
        FILE *file = fopen(files[i], "r");
        if (!file) {
            printf("Error opening file: %s\n", files[i]);
            continue;
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        char *file_content = malloc(file_size + 1);
        fread(file_content, 1, file_size, file);
        fclose(file);
        
        cJSON *json_data = cJSON_Parse(file_content);
        free(file_content);
        
        if (json_data) {
            for (int j = 0; j < cJSON_GetArraySize(json_data); j++) {
                cJSON *item = cJSON_GetArrayItem(json_data, j);
                cJSON_AddItemToArray(merged_data, cJSON_Duplicate(item, 1));
            }
            cJSON_Delete(json_data);
        }
    }
    
    FILE *output = fopen(output_file, "w");
    if (output) {
        char *merged_json = cJSON_Print(merged_data);
        fprintf(output, "%s", merged_json);
        free(merged_json);
        fclose(output);
    }
    
    cJSON_Delete(merged_data);
}

char* extract_domain_from_url(const char* url) {
    const char *http_prefix = "http://";
    const char *https_prefix = "https://";
    const char *domain_start = NULL;

    if (strncmp(url, http_prefix, strlen(http_prefix)) == 0) {
        domain_start = url + strlen(http_prefix);
    } else if (strncmp(url, https_prefix, strlen(https_prefix)) == 0) {
        domain_start = url + strlen(https_prefix);
    } else {
        domain_start = url;
    }

    const char *domain_end = strchr(domain_start, '/');
    
    if (domain_end == NULL) {
        domain_end = domain_start + strlen(domain_start);
    }

    size_t domain_len = domain_end - domain_start;
    char *domain = malloc(domain_len + 1);
    if (domain) {
        strncpy(domain, domain_start, domain_len);
        domain[domain_len] = '\0';  // Null-terminate the string
    }

    return domain;
}

void analyze_data(cJSON *data, int *depths, int *times, int *site_counts, FILE *csv_file) {
    int total_pages = 0;
    int total_links = 0;
    int total_time_ms = 0;
    
    cJSON *domains = cJSON_CreateObject();
    
    cJSON *item = NULL;
    cJSON_ArrayForEach(item, data) {
        total_pages++;
        total_links += cJSON_GetObjectItem(item, "linksExtracted")->valueint;
        total_time_ms += cJSON_GetObjectItem(item, "timeTakenMs")->valueint;
        
        int depth = cJSON_GetObjectItem(item, "depth")->valueint;
        depths[depth]++;
        
        int time_taken = cJSON_GetObjectItem(item, "timeTakenMs")->valueint;
        times[depth] += time_taken;
        
        site_counts[depth]++;
        
        char *url = cJSON_GetObjectItem(item, "url")->valuestring;
        char *domain = extract_domain_from_url(url);
        cJSON *domain_count = cJSON_GetObjectItem(domains, domain);
        if (domain_count) {
            domain_count->valueint++;
        } else {
            cJSON_AddNumberToObject(domains, domain, 1);
        }
    }

    // Output data to CSV
    fprintf(csv_file, "Total Pages Crawled, Total Links Found, Total Time Taken (ms)\n");
    fprintf(csv_file, "%d, %d, %d\n", total_pages, total_links, total_time_ms);
    
    fprintf(csv_file, "Depth Distribution:\n");
    for (int i = 0; i < 100; i++) {
        if (depths[i] > 0) {
            fprintf(csv_file, "Depth %d, %d pages\n", i, depths[i]);
        }
    }
    
    fprintf(csv_file, "Domain Counts:\n");
    cJSON *domain_item = NULL;
    cJSON_ArrayForEach(domain_item, domains) {
        fprintf(csv_file, "%s, %d\n", domain_item->string, domain_item->valueint);
    }

    // printf("Total Pages Crawled: %d\n", total_pages);
    // printf("Total Links Found: %d\n", total_links);
    // printf("Total Time Taken: %d ms\n", total_time_ms);
    
    // printf("Depth Distribution:\n");
    // for (int i = 0; i < 100; i++) {
    //     if (depths[i] > 0) {
    //         printf("Depth %d: %d pages\n", i, depths[i]);
    //     }
    // }
    
    // printf("Domain Counts:\n");
    // cJSON_ArrayForEach(domain_item, domains) {
    //     printf("%s: %d\n", domain_item->string, domain_item->valueint);
    // }
    
    cJSON_Delete(domains);
}

void generate_depth_distribution_plot(int depths[]) {
    FILE *data_file = fopen("plots/depth_distribution.dat", "w");
    for (int i = 0; i < 100; i++) {
        if (depths[i] > 0) {
            fprintf(data_file, "%d %d\n", i, depths[i]);
        }
    }
    fclose(data_file);
    
    system("gnuplot -e \"set terminal png; set output 'plots/depth_distribution.png'; plot 'plots/depth_distribution.dat' using 1:2 with boxes\"");
}

void generate_time_per_thread_plot(int times[], int num_threads) {
    FILE *data_file = fopen("plots/time_per_thread.dat", "w");
    for (int i = 0; i < num_threads; i++) {
        fprintf(data_file, "%d %d\n", i, times[i]);
    }
    fclose(data_file);
    
    system("gnuplot -e \"set terminal png; set output 'plots/time_per_thread.png'; plot 'plots/time_per_thread.dat' using 1:2 with linespoints\"");
}

void generate_sites_per_thread_plot(int site_counts[], int num_threads) {
    FILE *data_file = fopen("plots/sites_per_thread.dat", "w");
    for (int i = 0; i < num_threads; i++) {
        fprintf(data_file, "%d %d\n", i, site_counts[i]);
    }
    fclose(data_file);
    
    system("gnuplot -e \"set terminal png; set output 'plots/sites_per_thread.png'; plot 'plots/sites_per_thread.dat' using 1:2 with linespoints\"");
}

int main() {
    // Create a folder for storing the plots
    system("mkdir -p plots");

    // Input and output files
    const char *files[] = {"crawl_data_1.json", "crawl_data_2.json"};
    int num_files = 2;
    load_and_merge_json(files, num_files, "merged_data.json");

    // Read merged data
    FILE *merged_file = fopen("merged_data.json", "r");
    fseek(merged_file, 0, SEEK_END);
    long file_size = ftell(merged_file);
    fseek(merged_file, 0, SEEK_SET);
    
    char *file_content = malloc(file_size + 1);
    fread(file_content, 1, file_size, merged_file);
    fclose(merged_file);
    
    cJSON *data = cJSON_Parse(file_content);
    free(file_content);
    
    if (data) {
        int depths[100] = {0};
        int times[100] = {0};
        int site_counts[100] = {0};
        
        // Open CSV file for output
        FILE *csv_file = fopen("analysis_output.csv", "w");
        if (csv_file) {
            analyze_data(data, depths, times, site_counts, csv_file);
            fclose(csv_file);
        }
        
        // Generate plots
        generate_depth_distribution_plot(depths);
        generate_time_per_thread_plot(times, 100);  // assuming 100 threads
        generate_sites_per_thread_plot(site_counts, 100);  // assuming 100 threads
        
        cJSON_Delete(data);
    }
    printf("Your data has been saved in 'analysis_output.csv' and graphs are in the 'plot' folder.\n");

    return 0;
}
