/*
 * Copyright (c) 2026, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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