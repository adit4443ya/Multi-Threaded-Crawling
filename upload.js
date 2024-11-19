const { MongoClient, ServerApiVersion } = require("mongodb");
const fs = require("fs");
const path = require("path");

const uri = "mongodb+srv://tinkerquestproject:qwqw111@cluster0.fmcz6yd.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0";
const client = new MongoClient(uri, {
    serverApi: {
        version: ServerApiVersion.v1,
        strict: true,
        deprecationErrors: true,
    }
});

// Directory containing the JSON files
const dataDir = path.join(__dirname, "crawler_output");

async function uploadData() {
    try {
        await client.connect();
        console.log("Connected to MongoDB");

        const db = client.db("crawler");
        const collection = db.collection("data");

        const files = fs.readdirSync(dataDir);

        for (const file of files) {
            const filePath = path.join(dataDir, file);
            let fileContents = fs.readFileSync(filePath, "utf8");

            // Add closing bracket if necessary
            fileContents = finalizeJsonContent(fileContents, filePath);

            // Clean up the content by removing extra commas or malformed content
            fileContents = removeTrailingCommas(fileContents);
            fileContents = sanitizeJSON(fileContents);

            // If the content is invalid, skip or fix it
            if (!fileContents) {
                console.error(`Skipping invalid content in ${file}`);
                continue;
            }

            try {
                const jsonData = JSON.parse(fileContents);
                // Insert the data into MongoDB
                for (const data of jsonData) {
                    await collection.insertOne(data);
                    console.log(`Uploaded data from ${file}`);
                }
            } catch (parseError) {
                console.error(`Error parsing JSON in ${file}: ${parseError.message}`);
            }
        }

        console.log("All data uploaded!");
    } catch (err) {
        console.error("Error uploading data:", err);
    } finally {
        await client.close();
        console.log("MongoDB connection closed.");
    }
}

// Function to add a closing bracket "]" to JSON files that are missing it
function finalizeJsonContent(content, filePath) {
    // Check if the content is a non-empty string and doesn't end with ']'
    if (content && content.trim() && !content.trim().endsWith(']')) {
        console.log(`Adding closing bracket to ${filePath}`);
        content = content.trim() + "\n]"; // Append closing bracket
        fs.writeFileSync(filePath, content, "utf8"); // Overwrite file with fixed content
    }
    return content;
}

// Function to remove extra commas from JSON data
function removeTrailingCommas(data) {
    // Remove any trailing commas after objects or arrays
    return data.replace(/,(\s*[\}\]])/g, "$1");
}

// Function to sanitize and ensure valid JSON
function sanitizeJSON(data) {
    // Remove any invalid 'undefined' or non-JSON content
    if (data === "undefined" || !data.trim()) {
        console.error("Empty or undefined JSON content found.");
        return null;
    }
    return data;
}

uploadData();
