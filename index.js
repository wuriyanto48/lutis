const lutis = require('./lib');

const fs = require('fs');

let filePath = 'pp.webp';

let res = lutis.randomPixelJpeg(5, 5);
fs.writeFile('out.jpeg', res, 'binary', (err) => {
    if (err) {
        console.log('error... ', err);
    } else {
        console.log(res);
    }
});

// fs.readFile(filePath, null, (err, data) => {
//     if (err) {
//         console.log(err);
//     } else {
//         let res = lutis.grayFilterJpeg(data);
        
//         fs.writeFile('gray_out.jpeg', res, 'binary', (err) => {
//             if (err) {
//                 console.log('------', err);
//             }
//         });
//     }
// });

// fs.readFile(filePath, null, (err, data) => {
//     if (err) {
//         console.log(err);
//     } else {

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
//         fs.writeFile('out.webp', res, 'binary', (err) => {
//             if (err) {
//                 console.log(err);
//             }
//         });

//         // fs.createWriteStream('out.png').write(res);
        
//     }
// });

