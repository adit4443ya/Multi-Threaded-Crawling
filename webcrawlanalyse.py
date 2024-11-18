import os
import json
from collections import Counter
from urllib.parse import urlparse
import matplotlib.pyplot as plt


def load_json_file(filepath):
    """
    Load and parse a JSON file.
    """
    with open(filepath, "r") as file:
        try:
            data = json.load(file)
            return data
        except json.JSONDecodeError:
            print(f"Error parsing the file {filepath}.")
            return []


def merge_json_files(file1, file2, output_file):
    """
    Merge two JSON files into one.
    """
    all_data = []

    # Open the first JSON file and load its content
    with open(file1, "r") as f1:
        try:
            data1 = json.load(f1)
            all_data.extend(data1)  # Add data from the first file to the list
        except json.JSONDecodeError:
            print(f"Error decoding JSON in file: {file1}")

    # Open the second JSON file and load its content
    with open(file2, "r") as f2:
        try:
            data2 = json.load(f2)
            all_data.extend(data2)  # Add data from the second file to the list
        except json.JSONDecodeError:
            print(f"Error decoding JSON in file: {file2}")

    # Merge the data and write it to the output file
    with open(output_file, "w") as output:
        json.dump(all_data, output, indent=4)

    print(f"All data has been merged into {output_file}.")


def analyze_data(data):
    """
    Perform analysis on the crawled data.
    """
    total_pages = len(data)
    total_links = sum(page.get("linksExtracted", 0) for page in data)  # Adjusted based on your data structure
    total_time_ms = sum(page.get("timeTakenMs", 0) for page in data)  # Total time in milliseconds
    depths = [page.get("depth", 0) for page in data]
    depth_distribution = Counter(depths)

    domains = [urlparse(page.get("url", "")).netloc for page in data]
    domain_counts = Counter(domains).most_common(10)

    # Calculate time taken in seconds
    time_taken_seconds = total_time_ms / 1000.0

    # Calculate unique sites crawled
    unique_sites = set(domains)
    num_sites_crawled = len(unique_sites)

    # Calculate total time taken by each domain (for graphical representation)
    domain_times = {}
    for page in data:
        domain = urlparse(page.get("url", "")).netloc
        domain_times[domain] = domain_times.get(domain, 0) + page.get("timeTakenMs", 0)

    return {
        "total_pages": total_pages,
        "total_links": total_links,
        "depth_distribution": depth_distribution,
        "top_domains": domain_counts,
        "time_taken_seconds": time_taken_seconds,
        "num_sites_crawled": num_sites_crawled,
        "domain_times": domain_times
    }


def visualize_data(analysis_results):
    """
    Visualize the analysis results using matplotlib.
    """
    # Create a main folder for storing images if it doesn't exist
    output_dir = "analysis_images"
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Bar Chart: Depth Distribution
    depths, counts = zip(*analysis_results["depth_distribution"].items())
    plt.figure(figsize=(10, 6))
    plt.bar(depths, counts, color='skyblue')
    plt.xlabel("Depth Level")
    plt.ylabel("Number of Pages")
    plt.title("Depth Distribution of Crawled Pages")
    plt.xticks(depths)
    depth_chart_path = os.path.join(output_dir, "depth_distribution.png")
    plt.savefig(depth_chart_path)
    plt.show()

    # Pie Chart: Top Domains
    labels, counts = zip(*analysis_results["top_domains"])
    plt.figure(figsize=(8, 8))
    plt.pie(counts, labels=labels, autopct='%1.1f%%', startangle=140, colors=plt.cm.Paired.colors)
    plt.title("Top 10 Domains Crawled")
    top_domains_chart_path = os.path.join(output_dir, "top_domains.png")
    plt.savefig(top_domains_chart_path)
    plt.show()

    # Pie Chart: Unique Sites Crawled
    unique_sites = analysis_results["domain_times"]
    unique_sites_labels = list(unique_sites.keys())
    unique_sites_counts = list(unique_sites.values())
    plt.figure(figsize=(8, 8))
    plt.pie(unique_sites_counts, labels=unique_sites_labels, autopct='%1.1f%%', startangle=140, colors=plt.cm.Paired.colors)
    plt.title("Proportion of Pages Crawled by Domain")
    unique_sites_chart_path = os.path.join(output_dir, "unique_sites.png")
    plt.savefig(unique_sites_chart_path)
    plt.show()

    # Bar Chart: Total Time Taken per Domain
    domains, times = zip(*analysis_results["domain_times"].items())
    plt.figure(figsize=(12, 8))
    plt.bar(domains, times, color='lightgreen')
    plt.xlabel("Domain")
    plt.ylabel("Total Time Taken (ms)")
    plt.title("Total Time Taken per Domain")
    plt.xticks(rotation=90)
    time_per_domain_chart_path = os.path.join(output_dir, "time_per_domain.png")
    plt.savefig(time_per_domain_chart_path)
    plt.show()

    # Notify user where the files are saved
    print(f"Visualizations saved in '{output_dir}'.")


def main():
    """
    Main function to load, merge, analyze, and visualize crawled data.
    """
    # Path to the folder containing thread JSON files
    file1 = "crawl_data_1.json"
    file2 = "crawl_data_2.json"
    output_file = "crawl_data.json"  # Path to the merged JSON file

    # Step 1: Merge JSON files from multiple threads into one
    print("Merging JSON files...")
    merge_json_files(file1, file2, output_file)

    # Step 2: Load Merged Data
    print("Loading data from merged JSON file...")
    data = load_json_file(output_file)
    if not data:
        print("No data found in the file. Exiting...")
        return
    print(f"Loaded {len(data)} pages from {output_file}.")

    # Step 3: Analyze Data
    print("Analyzing data...")
    analysis_results = analyze_data(data)
    print(f"Total Pages Crawled: {analysis_results['total_pages']}")
    print(f"Total Links Found: {analysis_results['total_links']}")
    print(f"Time Taken for Crawling: {analysis_results['time_taken_seconds']:.2f} seconds")
    print(f"Number of Unique Sites Crawled: {analysis_results['num_sites_crawled']}")
    print(f"Depth Distribution: {dict(analysis_results['depth_distribution'])}")
    print(f"Top 10 Domains: {analysis_results['top_domains']}")

    # Step 4: Visualize Data
    print("Visualizing data...")
    visualize_data(analysis_results)


if __name__ == "__main__":
    main()
