# Makefile
CC = g++
OBJ = serialization_experiment write_fixed_len_pages read_fixed_len_page \
		csv2heapfile scan insert update delete select \
		csv2colstore select2 select3

serialization_experiment: SerializationExperiment.o Record.o utils.o
	$(CC) -o $@ $^

write_fixed_len_pages: WriteFixedLenPages.o utils.o Record.o Page.o HeapFile.o
	$(CC) -o $@ $^

read_fixed_len_page: ReadFixedLenPage.o utils.o Record.o Page.o HeapFile.o
	$(CC) -o $@ $^

csv2heapfile: csv2heapfile.cc library.o
	$(CC) -o $@ $< library.o

scan: scan.cc library.o
	$(CC) -o $@ $< library.o

insert: insert.cc library.o
	$(CC) -o $@ $< library.o

update: update.cc library.o
	$(CC) -o $@ $< library.o

delete: delete.cc library.o
	$(CC) -o $@ $< library.o

select: select.cc library.o
	$(CC) -o $@ $< library.o

csv2colstore: csv2colstore.cc library.o
	$(CC) -o $@ $< library.o

select2: select2.cc library.o
	$(CC) -o $@ $< library.o

select3: select3.cc library.o
	$(CC) -o $@ $< library.o
clean:
	rm -f $(OBJ) *.csv *.page
