all:
	gcc vxi11_clnt.c vxi11_xdr.c test_cmd.cc -o test_cmd
clean:
	rm -f *.o test_cmd
