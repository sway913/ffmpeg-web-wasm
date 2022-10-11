const fs = require('fs');
const path = require('path');

require('esbuild').serve({
  servedir: '.',
  port: 8099,
}, {
  entryPoints: ['main.js'],
  bundle: true,
  format: 'esm',
  outfile: 'bundle.js',
})