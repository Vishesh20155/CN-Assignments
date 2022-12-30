# Remove epoll_server for evaluation on mac

default: fork_server thread_server concurrent_client poll_server select_server sequential_server sequential_client

sequential_client: sequential_client.c
	gcc -o sequential_client sequential_client.c 

sequential_server: sequential_server.c
	gcc -o sequential_server sequential_server.c 

concurrent_client: concurrent_client.c
	gcc -o concurrent_client concurrent_client.c -lpthread

fork_server: server_fork.c
	gcc -o fork_server server_fork.c 

thread_server: server_thread.c
	gcc -o thread_server server_thread.c -lpthread

select_server: server_select.c
	gcc -o select_server server_select.c 

poll_server: server_poll.c
	gcc -o poll_server server_poll.c 

	
clean:
	rm -f fork_server thread_server concurrent_client poll_server select_server sequential_server sequential_client

