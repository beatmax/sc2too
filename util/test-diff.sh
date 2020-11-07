#!/bin/bash

diff --unchanged-line-format='%L' --changed-group-format="<<<<<<< INPUT%c'\012'%<=======%c'\012'%>>>>>>>> OUTPUT%c'\012'" "$@"
