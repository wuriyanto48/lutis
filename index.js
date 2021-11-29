const lutis = require('./lib');

const fs = require('fs');
const { constants } = require('buffer');

let filePath = 'es.png';

// let res = lutis.randomPixelJpeg(20, 10);
// fs.writeFile('out.jpeg', res, 'binary', (err) => {
//     if (err) {
//         console.log('error... ', err);
//     } else {
//         console.log(res);
//     }
// });

//  lutis.drawCircle('.png', 400, 400, 90, {R:245, G:245, B:73}, (err, res) => {
//     if (err) {
//         console.log(err);
//     } else {
//         console.log(res.slice(0, 10));
//         fs.writeFile('out_circle.png', res, 'binary', (err) => {
//             if (err) {
//                 console.log(err);
//             }
//         });
//     }
    
// });

fs.readFile(filePath, null, (err, data) => {
    if (err) {
        console.log(err);
    } else {
        let tessdata_path = `${process.cwd()}/tessdata`;
        let res = lutis.ocrScan(tessdata_path, data);
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

//         lutis.gaussianBlur('.png', data, (err, res) => {
//             if (err) {
//                 console.log(err);
//             } else {
//                 console.log(data.slice(0, 10));
//                 console.log(res.slice(0, 10));
//                 fs.writeFile('out_blur.png', res, 'binary', (err) => {
//                     if (err) {
//                         console.log(err);
//                     }
//                 });
//             }
            
//         });

//         // let res = lutis.resizeMagick(200, 100, 2, data);
//         // let res = lutis.rotateMagick(90, data);
        
//         // let text = `Optionally the bitstream features can be read into config.input, 
//         // in case we need to know them in advance. For instance it can handy to 
//         // know whether the picture has some transparency at all. Note that this 
//         // will also parse the bitstream's header, and is therefore a good way of 
//         // knowing if the bitstream looks like a valid WebP one.`
        
//         // let text_size = 50;
//         // let res = lutis.drawTextMagick(text, 'Impact', text_size, data, 50, 50, {x: 50, y: 50}, {R:245, G:245, B:73}, "png");
//         // // let res = lutis.grayFilterWebp(data);

//         // // let res = lutis.jpegToWebp(data);
//         // console.log(res);
//         // fs.writeFile('out_text.png', res, 'binary', (err) => {
//         //     if (err) {
//         //         console.log(err);
//         //     }
//         // });

//         // fs.createWriteStream('out.png').write(res);
        
//     }
// });

