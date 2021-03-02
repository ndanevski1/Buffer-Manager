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

select: Select.o utils.o Record.o Page.o HeapFile.o CSVUtils.o
	$(CC) -o $@ $^

csv2colstore: CSV2ColStore.o utils.o Record.o Page.o HeapFile.o CSVUtils.o
	$(CC) -o $@ $^

select2: Select2.o utils.o Record.o Page.o HeapFile.o CSVUtils.o ColStore.o
	$(CC) -o $@ $^

select3: Select3.o utils.o Record.o Page.o HeapFile.o CSVUtils.o ColStore.o
	$(CC) -o $@ $^

all: $(OBJ)

clean:
	rm -f $(OBJ) *.csv *.page *.heap
