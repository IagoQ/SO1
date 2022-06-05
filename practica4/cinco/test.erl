-module(test).
-export([test/0]).
-import(client, [client/0]).
-import(broadcaster, [server/0]).



test() -> 
	BPid = spawn(broadcaster,server,[]),
	Subs = subscribe(BPid,10,[]),
	BPid ! {send, "ji"},
	timer:sleep(1),
	unsubscribe(BPid,Subs),
	BPid ! {send, "je"}.



subscribe(Broadcaster_pid, 0, Subbed) -> Subbed;
subscribe(Broadcaster_pid,N, Subbed) -> 
	CPid = spawn(client,client,[]),
	Broadcaster_pid ! {subscribe, CPid},
	subscribe(Broadcaster_pid,N-1, [CPid|Subbed]).

unsubscribe(Broadcaster_pid,[]) -> ok;
unsubscribe(Broadcaster_pid,[Sub|Subbed]) -> 
	Broadcaster_pid ! {unsubscribe,Sub},
	unsubscribe(Broadcaster_pid,Subbed).
