#!/usr/bin/env node
'use strict';
const { spawnSync } = require('child_process');
const r = spawnSync(require('..').bin('hermes'), process.argv.slice(2), { stdio: 'inherit' });
if (r.error) { console.error(r.error.message); process.exit(1); }
process.exit(r.status === null ? 1 : r.status);
