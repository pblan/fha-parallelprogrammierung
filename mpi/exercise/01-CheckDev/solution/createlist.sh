#!/bin/bash

ls /usr/bin | sort > files-in-usr-bin
less files-in-usr-bin
rm files-in-usr-bin
