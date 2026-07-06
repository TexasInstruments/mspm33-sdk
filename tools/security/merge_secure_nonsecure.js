/*
 * Copyright (c) 2026, Texas Instruments Incorporated - http://www.ti.com
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
 *
 */
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