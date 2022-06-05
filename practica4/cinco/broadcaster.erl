-module(broadcaster).
-export([server/0]).


server() ->
	loop([]).


loop(Subs) -> 
	receive 
		{send, Msg} -> broadcast(Msg,Subs),loop(Subs);
		{subscribe,Pid} -> loop([Pid|Subs]);
		{unsubscribe,Pid} -> loop(lists:filter(fun(Sub) -> Sub /= Pid end, Subs))
	end.


broadcast(Msg,[]) -> ok;
broadcast(Msg,[Sub|Subs]) -> Sub ! Msg, broadcast(Msg,Subs). 
