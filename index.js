const lutis = require('./lib');

const fs = require('fs');

let filePath = 'elasticsearch_logo.png';

fs.readFile(filePath, null, (err, data) => {
    if (err) {
        console.log(err);
    } else {

        let inData = new Uint8Array(data);

        let res = lutis.gaussianBlur('.png', data);
        console.log(data.slice(0, 10));
        console.log(res.slice(0, 10));

        // fs.writeFile('out.jpg', res, 'binary', (err) => {
        //     if (err) {
        //         console.log(err);
        //     }
        // });

        fs.createWriteStream('out.png').write(res);
    }
});

