#!/usr/bin/expect -f

# b) check that space after deleting a register
##     is reused
# b.3 worstfit

set timeout -1
set programName "library"
set filename "test"

# delete all files starting with test
# file delete [glob test*]
spawn rm -f $filename.db $filename.ind $filename.lst

# call program
#spawn valgrind ./$programName best_fit test
spawn valgrind --leak-check=full ./$programName worst_fit test
expect "Type command and argument/s."
expect "exit"

# ADD BOOKS
# add first book
send  "add 12346|978-2-12345680-3|El Quijote|Catedra\r"
expect "Record with BookID=12346 has been added to the database"
expect "exit"
send "printInd\n"
expect "exit"

# add second book
send  "add 12345|978-2-12345086-3|La busca|Catedra\r"
expect "Record with BookID=12345 has been added to the database"
expect "exit"
send "printInd\n"
expect "exit"

# add third book
send  "add 12347|978-2-12345680-4|el quijote II|catedra\r"
expect "Record with BookID=12347 has been added to the database"
expect "exit"
send "printInd\n"
expect "exit"

# add fourth book
send  "add 12348|978-2-12345086-3|la encuentra|catedra\r"
expect "Record with BookID=12348 has been added to the database"
expect "exit"

# check index
# print index
puts "------------------------"
send "printInd\n"
expect "Entry #0"
expect "    key: #12345"
expect "    offset: #46"
expect "    size: #36"
expect "Entry #1"
expect "    key: #12346"
expect "    offset: #0"
expect "    size: #38"
expect "Entry #2"
expect "    key: #12347"
expect "    offset: #90"
expect "    size: #41"
expect "Entry #3"
expect "    key: #12348"
expect "    offset: #139"
expect "    size: #40"
expect "exit"

#delete book 12347
send "del 12347\r"
expect "Record with BookID=12347 has been deleted"
expect "exit"
send "printInd\n"
expect "Entry #0"
expect "    key: #12345"
expect "    offset: #46"
expect "    size: #36"
expect "Entry #1"
expect "    key: #12346"
expect "    offset: #0"
expect "    size: #38"
expect "Entry #2"
expect "    key: #12348"
expect "    offset: #139"
expect "    size: #40"
expect "exit"

send "printLst\n"
expect "Entry #0"
expect "    offset: #90"
expect "    size: #41"
expect "exit"

#delete book 12345
send "del 12345\r"
expect "Record with BookID=12345 has been deleted"
expect "exit"
send "printInd\n"
expect "Entry #0"
expect "    key: #12346"
expect "    offset: #0"
expect "    size: #38"
expect "Entry #1"
expect "    key: #12348"
expect "    offset: #139"
expect "    size: #40"
expect "exit"

send "printLst\n"
expect "Entry #0"
expect "    offset: #90"
expect "    size: #41"
expect "Entry #1"
expect "    offset: #46"
expect "    size: #36"
expect "exit"

# add another time the deleted book
send  "add 12345|978-2-12345086-3|La busca|Catedra\r"
expect "Record with BookID=12345 has been added to the database"
expect "exit"

# check index
# print index
puts "------------------------"
send "printInd\n"
expect "Entry #0"
expect "    key: #12345"
expect "    offset: #90"
expect "    size: #36"
expect "Entry #1"
expect "    key: #12346"
expect "    offset: #0"
expect "    size: #38"
expect "Entry #2"
expect "    key: #12348"
expect "    offset: #139"
expect "    size: #40"
expect "exit"

send "exit\n"
expect "all done"