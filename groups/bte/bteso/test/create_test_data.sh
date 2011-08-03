#!/bin/sh
comdb2sql bdetstdb 'INSERT INTO user (last_name,first_name,zipcode)
                    VALUES("Doe", "John", 11021)'
comdb2sql bdetstdb 'INSERT INTO user (last_name,first_name,zipcode) 
                    VALUES("Smith", "Jane", 10001)'
comdb2sql bdetstdb 'INSERT INTO user (last_name,first_name,zipcode) 
                    VALUES("Brown", "Sally", 10022)'

comdb2sql bdetstdb 'INSERT INTO bdetst01 (uuid, firstname, lastname, notes, notes2, added, lastupdate)
                    VALUES(1, "George", "Washington", "myNotes", NULL, now(), NULL)'
comdb2sql bdetstdb 'INSERT INTO bdetst01 (uuid, firstname, lastname, notes, notes2, added, lastupdate)
                    VALUES(2, "John", "Adams", "myNotes", "myNotes2", now(), NULL)'
comdb2sql bdetstdb 'INSERT INTO bdetst01 (uuid, firstname, lastname, notes, notes2, added, lastupdate)
                    VALUES(3, "Thomas", "Jefferson", "myNotes", NULL, now(), NULL)'
comdb2sql bdetstdb 'INSERT INTO bdetst01 (uuid, firstname, lastname, notes, notes2, added, lastupdate)
                    VALUES(4, "James", "Madison", "myNotes", NULL, now(), NULL)'
comdb2sql bdetstdb 'INSERT INTO bdetst01 (uuid, firstname, lastname, notes, notes2, added, lastupdate)
                    VALUES(5, "James", "Monroe", "myNotes", NULL, now(), NULL)'
comdb2sql bdetstdb 'INSERT INTO bdetst01 (uuid, firstname, lastname, notes, notes2, added, lastupdate)
                    VALUES(6, "Andrew", "Jackson", "myNotes", "myNotes2", now(), NULL)'
comdb2sql bdetstdb 'INSERT INTO bdetst01 (uuid, firstname, lastname, notes, notes2, added, lastupdate)
                    VALUES(7, "Martin", "Buren", "myNotes", NULL, now(), NULL)'
comdb2sql bdetstdb 'INSERT INTO bdetst01 (uuid, firstname, lastname, notes, notes2, added, lastupdate)
                    VALUES(8, "William", "Harrison", "myNotes", NULL, now(), NULL)'



