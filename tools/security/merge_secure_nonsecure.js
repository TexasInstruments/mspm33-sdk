// This file takes two filenames as arguments, file1.txt and file2.txt, and generates a new file named file1_full.txt, which contains the content of file1 except the last line and the full content of file2. This file is used for examples/security projects, to create fully merged single loadable file.

const fs = require('fs');
const path = require('path');

if (process.argv.length < 4){
    console.error('Usage: node merge_secure_nonsecure.js <file1> <file2>')
    process.exit(1);
}

const file1 = process.argv[2];
const file2 = process.argv[3];

let data1, data2;

try{
    data1 = fs.readFileSync(file1,'utf-8').trim().split('\n');
    data2 = fs.readFileSync(file2,'utf-8').trim();
} catch (err) {
    console.error('Error reading input files:',err.message);
    process.exit(1);
}

if (data1.length > 0){data1 = data1.slice(0,-1);}

const merged = data1.join('\n') + (data1.length ? '\n' : '') + data2;

const { dir, name, ext } = path.parse(file1);
const outputFile = path.join(dir, `${name}_full${ext}`);

try{
    fs.writeFileSync(outputFile, merged, 'utf-8');
    console.log(`Created: ${outputFile}`);
} catch (err) {
    console.error('Error writing output file:', err.message);
    process.exit(1);
}