const fs = require("fs");
const path = require("path");

/**
 * Combines two binary files into one
 * @param {string} file1Path - Path to the first binary file
 * @param {string} file2Path - Path to the second binary file
 * @param {string} outputPath - Path for the combined output file
 */
function combineBinaryFiles(file1Path, file2Path, outputPath) {
  try {
    // Check if input files exist
    if (!fs.existsSync(file1Path)) {
      throw new Error(`File 1 does not exist: ${file1Path}`);
    }

    if (!fs.existsSync(file2Path)) {
      throw new Error(`File 2 does not exist: ${file2Path}`);
    }

    // Read both binary files
    const file1Data = fs.readFileSync(file1Path);
    const file2Data = fs.readFileSync(file2Path);

    // Create output directory if it doesn't exist
    const outputDir = path.dirname(outputPath);
    if (!fs.existsSync(outputDir)) {
      fs.mkdirSync(outputDir, { recursive: true });
    }

    // Combine the buffers
    const combinedData = Buffer.concat([file1Data, file2Data]);

    // Check if the combined data length is a multiple of 16
    // If not, pad with 0xFF bytes to make it a multiple of 16
    let finalData = combinedData;
    const remainder = combinedData.length % 16;

    if (remainder !== 0) {
      const paddingLength = 16 - remainder;
      const padding = Buffer.alloc(paddingLength, 0xff);
      finalData = Buffer.concat([combinedData, padding]);
      console.log(
        `   Added ${paddingLength} bytes of 0xFF padding to make length a multiple of 16`
      );
    }

    // Write the final data to output file
    fs.writeFileSync(outputPath, finalData);

    console.log(`✅ Successfully combined files:`);
    console.log(`   File 1: ${file1Path} (${file1Data.length} bytes)`);
    console.log(`   File 2: ${file2Path} (${file2Data.length} bytes)`);
    console.log(`   Output: ${outputPath} (${finalData.length} bytes)`);
    if (finalData.length !== combinedData.length) {
      console.log(`   Original combined size: ${combinedData.length} bytes`);
      console.log(`   Final size after padding: ${finalData.length} bytes`);
    }

    return true;
  } catch (error) {
    console.error(`❌ Error combining files: ${error.message}`);
    return false;
  }
}

// Example usage
if (require.main === module) {
  // Get command line arguments
  const args = process.argv.slice(2);

  if (args.length !== 3) {
    console.log(
      "Usage: node combine-bins.js <file1.bin> <file2.bin> <output.bin>"
    );
    console.log(
      "Example: node combine-bins.js input1.bin input2.bin combined.bin"
    );
    process.exit(1);
  }

  const [file1Path, file2Path, outputPath] = args;

  const success = combineBinaryFiles(file1Path, file2Path, outputPath);
  process.exit(success ? 0 : 1);
}

module.exports = { combineBinaryFiles };