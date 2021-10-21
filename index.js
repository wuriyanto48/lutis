const lutis = require('./lib');

const fs = require('fs');

let filePath = 'pp.jpg';

let res = lutis.generateJpeg('out.jpeg');
fs.writeFile('out.jpeg', res, 'binary', (err) => {
    if (err) {
        console.log(err);
    }
});

fs.readFile(filePath, null, (err, data) => {
    if (err) {
        console.log(err);
    } else {
        let res = lutis.grayFilterJpeg(data);
        
        fs.writeFile('gray_out.jpeg', res, 'binary', (err) => {
            if (err) {
                console.log(err);
            }
        });
    }
});

// fs.readFile(filePath, null, (err, data) => {
//     if (err) {
//         console.log(err);
//     } else {

//         let inData = new Uint8Array(data);

//         // lutis.gaussianBlur('.png', data, (err, res) => {
//         //     if (err) {
//         //         console.log(err);
//         //     } else {
//         //         console.log(data.slice(0, 10));
//         //         console.log(res.slice(0, 10));
//         //         fs.writeFile('out.png', res, 'binary', (err) => {
//         //             if (err) {
//         //                 console.log(err);
//         //             }
//         //         });
//         //     }
            
//         // });

//         // lutis.drawCircle('.png', 400, 400, 90, {R:245, G:245, B:73}, (err, res) => {
//         //     if (err) {
//         //         console.log(err);
//         //     } else {
//         //         console.log(res.slice(0, 10));
//         //         fs.writeFile('out.png', res, 'binary', (err) => {
//         //             if (err) {
//         //                 console.log(err);
//         //             }
//         //         });
//         //     }
            
//         // });

//         // let res = lutis.resizeMagick(100, 800, 0.5, data);
        
//         // let res = lutis.drawTextMagick('hello world', data);
//         let res = lutis.decodeWebp(data);
//         console.log(res);

//         // fs.createWriteStream('out.png').write(res);
        
//     }
// });

