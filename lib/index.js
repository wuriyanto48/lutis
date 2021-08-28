let lutis;

try {
    lutis = require('../build/Release/lutis.node');
} catch(e) {
    if (e.code == 'MODULE_NOT_FOUND') throw e;
    lutis = require('../build/Debug/lutis.node');
}

module.exports = lutis;