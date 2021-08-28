const lutis = require('./lib');

const fs = require('fs');

let filePath = 'pp.jpg';

fs.readFile(filePath, null, (err, data) => {
    if (err) {
        console.log(err);
    } else {
        // console.log(Buffer.isBuffer(data));
        const array = new Uint8Array(data);
        // console.log(array);

        lutis.readFile(data);
    }
});

