-module(hello).
-export([init/0,hello/0]).
-on_load(reload/0).

hello() ->
	case whereis(hellopid) of 
		undefined -> register(hellopid, self());
		_ -> unregister(hellopid), register(hellopid, self())
	end,

	receive 
		upgrade -> 
			io:fwrite("calling \n"),
			spawn(?MODULE,hello,[]),
			exit("reloaded")
	after 1000 -> ok end,
	io:fwrite("2 ~p~n", [case rand:uniform(10) of 10 -> uno; _ -> self() end]),
	hello().

init() -> spawn(fun () -> hello() end).

reload() ->
	case whereis(hellopid) of 
		undefined -> ok;
		Pid -> io:fwrite("reloading \n"),Pid ! upgrade, ok
	end.