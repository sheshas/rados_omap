RADOS OMAP Bench
-----------------

This is a 'C' RADOS client to access and manipulate Objects OMAP
structure.  This provides a table interface that enables one to
create a table by specifying rows and columns.

The way data of the table is stored can be manipulated by specifiying
different naming schemes (Will be implemented soon), such as Row Major,
Column Major and Full Row.

User can perform different operations on the table like row inserts,
updates, row scans and column scans similar to YCSB workload (Will be
implemented soon)

Why 'C' and not 'C++'?
Simply because I am more comfortable in coding in 'C' rather than
'C++'.

Usage: ./rados_table <options>
	-p : Pool Name
	-r : Number of Rows
	-c : Number of Columns
	-s : Data size

Makefile supports -DDEBUG flag to turn on/off debugging.
