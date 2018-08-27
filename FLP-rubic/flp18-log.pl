%% 
%% FLP - projekt 2: Rubikova kostka
%% Michal Gabonay (xgabon00)
%% 2018
%% VUT FIT
%%

% ------------------------------------taken from input2.pl (study materials to FLP)----------------------------

%Reads line from stdin, terminates on LF or EOF.
read_line(L,C) :-
	get_char(C),
	(isEOFEOL(C), L = [], !;
		read_line(LL,_),% atom_codes(C,[Cd]),
		[C|LL] = L).

%Tests if character is EOF or LF.
isEOFEOL(C) :-
	C == end_of_file;
	(char_code(C,Code), Code==10).

read_lines(Ls) :-
	read_line(L,C),
	( C == end_of_file, Ls = [] ;
	  read_lines(LLs), Ls = [L|LLs]
	).

% rozdeli radek na podseznamy
split_line([],[[]]) :- !.
split_line([' '|T], [[]|S1]) :- !, split_line(T,S1).
split_line([32|T], [[]|S1]) :- !, split_line(T,S1).    % aby to fungovalo i s retezcem na miste seznamu
split_line([H|T], [[H|G]|S1]) :- split_line(T,[G|S1]). % G je prvni seznam ze seznamu seznamu G|S1

% vstupem je seznam radku (kazdy radek je seznam znaku)
split_lines([],[]).
split_lines([L|Ls],[H|T]) :- split_lines(Ls,T), split_line(L,H).

% -------------------------------------------------------------------------------------------------------------

% transform input rubic cube into my inner represenattion
createCube([U1, U2, U3,
			U4, U5, U6,
			U7, U8, U9,
			F1, F2, F3, R1, R2, R3, B1, B2, B3, L1, L2, L3,
			F4, F5, F6, R4, R5, R6, B4, B5, B6, L4, L5, L6,
			F7, F8, F9, R7, R8, R9, B7, B8, B9, L7, L8, L9,
			D1, D2, D3,
			D4, D5, D6,
		    D7, D8, D9], 
      	  ( U1, U2, U3, U4, U5, U6, U7, U8, U9,
			D1, D2, D3, D4, D5, D6, D7, D8, D9,
			L7, L4, L1, L8, L5, L2, L9, L6, L3,
			R3, R6, R9, R2, R5, R8, R1, R4, R7,
			F1, F2, F3, F4, F5, F6, F7, F8, F9,
			B9, B8, B7, B6, B5, B4, B3, B2, B1 )).

% write rubic cube on output in required format
writeCube((U1,U2,U3,U4,U5,U6,U7,U8,U9,D1,D2,D3,D4,D5,D6,D7,D8,D9,L7,L4,L1,L8,L5,L2,L9,L6,L3,R3,R6,R9,R2,R5,R8,R1,R4,R7,F1,F2,F3,F4,F5,F6,F7,F8,F9,B9,B8,B7,B6,B5,B4,B3,B2,B1)):-
	write(U1), write(U2), write(U3), nl,
	write(U4), write(U5), write(U6), nl,
	write(U7), write(U8), write(U9), nl,

	write(F1), write(F2), write(F3), write(' '), write(R1), write(R2), write(R3), write(' '), write(B1), write(B2), write(B3), write(' '), write(L1), write(L2), write(L3), nl,
	write(F4), write(F5), write(F6), write(' '), write(R4), write(R5), write(R6), write(' '), write(B4), write(B5), write(B6), write(' '), write(L4), write(L5), write(L6), nl,
	write(F7), write(F8), write(F9), write(' '), write(R7), write(R8), write(R9), write(' '), write(B7), write(B8), write(B9), write(' '), write(L7), write(L8), write(L9), nl,

	write(D1), write(D2), write(D3), nl,
	write(D4), write(D5), write(D6), nl,
	write(D7), write(D8), write(D9), nl, nl.

solved((U,U,U,U,U,U,U,U,U,D,D,D,D,D,D,D,D,D,L,L,L,L,L,L,L,L,L,R,R,R,R,R,R,R,R,R,F,F,F,F,F,F,F,F,F,B,B,B,B,B,B,B,B,B)).

% rotate and save sides, which was rotated
rotate([], Cube, Cube).
rotate([NextRotation | Rotation], Cube, NewState) :- rotate(Rotation, CurrentState, NewState), rotateside(NextRotation, Cube, CurrentState).

% try rotate cube, until it will be solved, every side have the same color, in SOLUTIN is array of which sides have to be rotated, step by step
solveCube(SOLUTION, CUBE) :- rotate(SOLUTION, CUBE, C), solved(C).

% write every step(new cube) from sulution
writeSteps([], _).
writeSteps([H|T], Cube) :- rotateside(H, Cube, NewCube), writeCube(NewCube), writeSteps(T, NewCube).

%% rotate up-side of cube by 90° clockwise 
rotateside(
    up,
    ( 	U1, U2, U3, U4, U5, U6, U7, U8, U9,    
        D1, D2, D3, D4, D5, D6, D7, D8, D9,    
        L1, L2, L3, L4, L5, L6, L7, L8, L9,     
        R1, R2, R3, R4, R5, R6, R7, R8, R9,    
        F1, F2, F3, F4, F5, F6, F7, F8, F9,     
        B1, B2, B3, B4, B5, B6, B7, B8, B9 ),

    ( 	U7, U4, U1, U8, U5, U2, U9, U6, U3,    
        D1, D2, D3, D4, D5, D6, D7, D8, D9,     
        L1, L2, F1, L4, L5, F2, L7, L8, F3,     
        B7, R2, R3, B8, R5, R6, B9, R8, R9,     
        R7, R4, R1, F4, F5, F6, F7, F8, F9,     
        B1, B2, B3, B4, B5, B6, L9, L6, L3 )).

%% rotate down-side of cube by 90° clockwise 
rotateside(
    down,
    ( 	U1, U2, U3, U4, U5, U6, U7, U8, U9,    
        D1, D2, D3, D4, D5, D6, D7, D8, D9,    
        L1, L2, L3, L4, L5, L6, L7, L8, L9,    
        R1, R2, R3, R4, R5, R6, R7, R8, R9,     
        F1, F2, F3, F4, F5, F6, F7, F8, F9,     
        B1, B2, B3, B4, B5, B6, B7, B8, B9 ),

    (   U1, U2, U3, U4, U5, U6, U7, U8, U9,     
        D3, D6, D9, D2, D5, D8, D1, D4, D7,     
        F7, L2, L3, F8, L5, L6, F9, L8, L9,     
        R1, R2, B1, R4, R5, B2, R7, R8, B3,    
        F1, F2, F3, F4, F5, F6, R9, R6, R3,    
        L7, L4, L1, B4, B5, B6, B7, B8, B9 )).

%% rotate left-side of cube by 90° clockwise 
rotateside(
    left,
    ( 	U1, U2, U3, U4, U5, U6, U7, U8, U9,     
        D1, D2, D3, D4, D5, D6, D7, D8, D9,     
        L7, L4, L1, L8, L5, L2, L9, L6, L3,     
        R1, R2, R3, R4, R5, R6, R7, R8, R9,    
        F1, F2, F3, F4, F5, F6, F7, F8, F9,    
        B1, B2, B3, B4, B5, B6, B7, B8, B9 ),

    (   B1, U2, U3, B4, U5, U6, B7, U8, U9,     
        F1, D2, D3, F4, D5, D6, F7, D8, D9,     
        L7, L4, L1, L8, L5, L2, L9, L6, L3,     
        R1, R2, R3, R4, R5, R6, R7, R8, R9,    
        U1, F2, F3, U4, F5, F6, U7, F8, F9,    
        D1, B2, B3, D4, B5, B6, D7, B8, B9 )).

%% rotate right-side of cube by 90° clockwise 
rotateside(
    right,
    (   U1, U2, U3, U4, U5, U6, U7, U8, U9,     
        D1, D2, D3, D4, D5, D6, D7, D8, D9,    
        L1, L2, L3, L4, L5, L6, L7, L8, L9,    
        R1, R2, R3, R4, R5, R6, R7, R8, R9,    
        F1, F2, F3, F4, F5, F6, F7, F8, F9,    
        B1, B2, B3, B4, B5, B6, B7, B8, B9 ),

    (   U1, U2, F3, U4, U5, F6, U7, U8, F9,     
        D1, D2, B3, D4, D5, B6, D7, D8, B9,     
        L1, L2, L3, L4, L5, L6, L7, L8, L9,     
        R7, R4, R1, R8, R5, R2, R9, R6, R3,     
        F1, F2, D3, F4, F5, D6, F7, F8, D9,    
        B1, B2, U3, B4, B5, U6, B7, B8, U9 )).

%% rotate front-side of cube by 90° clockwise 
rotateside(
    front,
    (   U1, U2, U3, U4, U5, U6, U7, U8, U9,     
        D1, D2, D3, D4, D5, D6, D7, D8, D9,     
        L1, L2, L3, L4, L5, L6, L7, L8, L9,   
        R1, R2, R3, R4, R5, R6, R7, R8, R9,    
        F1, F2, F3, F4, F5, F6, F7, F8, F9,    
        B1, B2, B3, B4, B5, B6, B7, B8, B9 ),

    (   U1, U2, U3, U4, U5, U6, L7, L8, L9,     
        R9, R8, R7, D4, D5, D6, D7, D8, D9,     
        L1, L2, L3, L4, L5, L6, D3, D2, D1,    
        R1, R2, R3, R4, R5, R6, U7, U8, U9,     
        F7, F4, F1, F8, F5, F2, F9, F6, F3,    
        B1, B2, B3, B4, B5, B6, B7, B8, B9 )
).

%% rotate back-side of cube by 90° clockwise 
rotateside(
    back,
    (   U1, U2, U3, U4, U5, U6, U7, U8, U9,   
        D1, D2, D3, D4, D5, D6, D7, D8, D9,    
        L1, L2, L3, L4, L5, L6, L7, L8, L9,     
        R1, R2, R3, R4, R5, R6, R7, R8, R9,     
        F1, F2, F3, F4, F5, F6, F7, F8, F9,     
        B1, B2, B3, B4, B5, B6, B7, B8, B9 ),
    
    (   L1, L2, L3, U4, U5, U6, U7, U8, U9,     
        D1, D2, D3, D4, D5, D6, R3, R2, R1,    
        D9, D8, D7, L4, L5, L6, L7, L8, L9,    
        U1, U2, U3, R4, R5, R6, R7, R8, R9,     
        F1, F2, F3, F4, F5, F6, F7, F8, F9,    
        B3, B6, B9, B2, B5, B8, B1, B4, B7 )).

main :-
		prompt(_, ''),
		read_lines(LL),
		split_lines(LL,S),
		flatten(S, K), % make one big array from input
		createCube(K, CUBE),
		writeCube(CUBE),
		solveCube(SOLUTION, CUBE),
		writeSteps(SOLUTION, CUBE),
		halt.