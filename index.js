const lutis = require('./lib');

const fs = require('fs');

let filePath = 're2.jpeg';

let res = lutis.randomPixelJpeg(20, 10);
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

//         // let res = lutis.resizeMagick(200, 100, 2, data);
//         // let res = lutis.rotateMagick(90, data);
        
//         let res = lutis.drawTextMagick('hello world', 'Arial', data, 50, 50, {x: 50, y: 50}, {R:245, G:245, B:73});
//         // let res = lutis.grayFilterWebp(data);

//         // let res = lutis.jpegToWebp(data);
//         console.log(res);
//         fs.writeFile('out_text.png', res, 'binary', (err) => {
//             if (err) {
//                 console.log(err);
//             }
//         });

//         // fs.createWriteStream('out.png').write(res);
        
//     }
// });

