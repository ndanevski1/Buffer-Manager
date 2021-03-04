# Makefile
CC = g++
OBJ = serialization_experiment write_fixed_len_pages read_fixed_len_page \
		csv2heapfile scan insert update delete select \
		csv2colstore select2 select3

OBJS = $(OBJDIR)/utils.o $(OBJDIR)/Record.o $(OBJDIR)/Page.o $(OBJDIR)/HeapFile.o $(OBJDIR)/CSVUtils.o $(OBJDIR)/ColStore.o
OBJDIR = obj
SRCDIR = src

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p obj; $(CC) -o $@ -c $<

serialization_experiment: $(OBJDIR)/SerializationExperiment.o $(OBJS)
	$(CC) -o $@ $^

write_fixed_len_pages: $(OBJDIR)/WriteFixedLenPages.o $(OBJS)
	$(CC) -o $@ $^

read_fixed_len_page: $(OBJDIR)/ReadFixedLenPage.o $(OBJS)
	$(CC) -o $@ $^

csv2heapfile: $(OBJDIR)/CSV2HeapFile.o $(OBJS)
	$(CC) -o $@ $^

scan: $(OBJDIR)/Scan.o $(OBJS)
	$(CC) -o $@ $^

insert: $(OBJDIR)/Insert.o $(OBJS)
	$(CC) -o $@ $^

update: $(OBJDIR)/Update.o $(OBJS)
	$(CC) -o $@ $^

delete: $(OBJDIR)/Delete.o $(OBJS)
	$(CC) -o $@ $^

select: $(OBJDIR)/Select.o $(OBJS)
	$(CC) -o $@ $^

csv2colstore: $(OBJDIR)/CSV2ColStore.o $(OBJS)
	$(CC) -o $@ $^

select2: $(OBJDIR)/Select2.o $(OBJS)
	$(CC) -o $@ $^

select3: $(OBJDIR)/Select3.o $(OBJS)
	$(CC) -o $@ $^

all: $(OBJ)

clean:
	rm -r -f $(OBJ) *.csv *.page *.heap *.col obj *.o
