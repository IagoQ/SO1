-module(client).
-export([client/0]).


client() -> 
	receive 
		X -> io:format("~p \n", [X]),client()
	end.

