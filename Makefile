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

csv2heapfile: CSV2HeapFile.o utils.o Record.o Page.o HeapFile.o CSVUtils.o
	$(CC) -o $@ $^ 

scan: Scan.o utils.o Record.o Page.o HeapFile.o
	$(CC) -o $@ $^

insert: Insert.o utils.o Record.o Page.o HeapFile.o CSVUtils.o
	$(CC) -o $@ $^

update: Update.o utils.o Record.o Page.o HeapFile.o CSVUtils.o
	$(CC) -o $@ $^

delete: Delete.o utils.o Record.o Page.o HeapFile.o CSVUtils.o
	$(CC) -o $@ $^

select: select.cc library.o
	$(CC) -o $@ $< library.o

csv2colstore: csv2colstore.cc library.o
	$(CC) -o $@ $< library.o

select2: select2.cc library.o
	$(CC) -o $@ $< library.o

select3: select3.cc library.o
	$(CC) -o $@ $< library.o

all: $(OBJ)

clean:
	rm -f $(OBJ) *.csv *.page *.heap
