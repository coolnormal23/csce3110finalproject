#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

class Board
{
	public:
		// notes on board representation
		// positive: white, negative: black
		// 0: none
		// 1: king
		// 2: pawn
		// 3: bishop
		// 4: knight
		// 5: rook
		// 6: queen
		// using LERF board representation.
		// see: www.chessprogramming.org/Square_Mapping_Considerations#Little-Endian-Rank-File_Mapping

		int board[64] = {
			5,   4,  3,  6,  1,  3,  4,  5,
			2,   0,  0,  0,  0,  0,  0,  2,
			0,   0,  0,  0,  0,  0,  0,  0,
			0,   0,  2,  0,  0,  2,  0,  0,
			0,   0,  0,  0,  0,  0,  0,  0,
			0,   0,  0,  0,  0,  0,  0,  0,
			-2, 0, 0, 0, 0, 0, -2, -2,
			-5, -4, -3, -6, -1, -3, -4, -5
		};

		int eval;
		bool check;
		bool turn;
		vector<Board> children;
		int nextboard;

		Board()
		{
			turn = true;
			eval = 0;
			check = false;
		};

		void printBoard()
		{
			for(int i = 0; i < 8; i++)
			{
				cout << 8-i << "  ";
				for(int j = (56-(i*8)); j < (64-(i*8)); j++)
				{
					int piece = board[j];
					switch(piece)
					{
						case 0:
							cout << "#";
							break;
						case 2:
							cout << "P";
							break;
						case -2:
							cout << "p";
							break;
						case 1:
							cout << "K";
							break;
						case -1:
							cout << "k";
							break;
						case 3:
							cout << "B";
							break;
						case -3:
							cout << "b";
							break;
						case 4:
							cout << "N";
							break;
						case -4:
							cout << "n";
							break;
						case 5:
							cout << "R";
							break;
						case -5:
							cout << "r";
							break;
						case 6:
							cout << "Q";
							break;
						case -6:
							cout << "q";
							break;
						default:
							//if this pops up, you fucked up!
							cout << "E";
					}	
				}
				cout << endl;
			}
			cout << endl << "   abcdefgh" << endl;
			evaluate();
			cout << "Evaluation: " << eval << endl;
			if(check)
			{
				cout << "Check!" << endl;
			}	
		};

		void evaluate()
		{
			eval = 0;

			for(int i = 0; i < 8; i++)
			{
				for(int j = (56-(i*8)); j < (64-(i*8)); j++)
				{
					int piece = board[j];
					switch(piece)
					{
						case 0:
							break;
						case 2:
							eval += 1;
							break;
						case -2:
							eval -= 1;
							break;
						case 1:
							eval += 99;
							break;
						case -1:
							eval -= 99;
							break;
						case 3:
						case 4:
							eval += 3;
							break;
						case -4:
						case -3:
							eval -= 3;
							break;
						case 5:
							eval += 5;
							break;
						case -5:
							eval -= 5;
							break;
						case 6:
							eval += 9;
							break;
						case -6:
							eval -= 9;
							break;
					}
				}
			}
		};

		bool legalCheck(string move)
		{
			// moves use algebraic notation
			// more info: en.wikipedia.org/wiki/Algebraic_notation_(chess)

			int file = -1;

			//first: parse destination square, and legality of that square
			int destination = 0;
			if(move[0] <= 'Z') //if a piece...
			{
				if(move[1] != 'x') //if no capture specified in 2nd character
				{
					if(move[2] <= 56) //if no file specified
					{
						destination += ((move[1]-97));
						destination += ((move[2]-49)*8);
					}
					else if(move[2] <= 104 && move[3] <= 56) //if file specified
					{
						if((move[1]-97) < 0 || (move[1]-97) > 7)
						{
							cout << "Invalid piece file declared" << endl;
							cout << move << endl << " ^" << endl;
							return false;
						}
						else
						{
							file = (move[1]-97);
							destination += ((move[2]-97));
							destination += ((move[3]-49)*8);
						}
					}
					else if(move[2] == 'x') //if capture and file specified
					{
						if((move[1]-97) < 0 || (move[1]-97) > 7)
						{
							cout << "Invalid piece file declared" << endl;
							cout << move << endl << " ^" << endl;
							return false;
						}
						else
						{
							file = (move[1]-97);
							destination += ((move[3]-97));
							destination += ((move[4]-49)*8);
						}
					}
				}
				if(move[1] == 'x') //if capture movement
				{
					destination += (move[2]-97);
					destination += ((move[3]-49)*8);
				}
			}
			else //pawn moves
			{
				if(move[1] == 'x') //if pawn capture
				{
					file = (move[0]-97);
					destination += ((move[2]-97));
					destination += ((move[3]-49)*8);
				}
				else
				{
					destination += ((move[0]-97));
					destination += ((move[1]-49)*8);
				}
			}

			//if destination square is out of board
			if(destination < 0 || destination > 63)
			{
				cout << "Destination not legal: " << destination << endl;
				return false;
			}

			cout << "Destination legal: " << destination << endl;

			//if whites turn
			if(turn)
			{
				if(board[destination] >0)
				{
					cout << "Destination has friendly piece." << endl;
					return false;
				}

				//what piece?
				switch(move[0])
				{
					//bishop
					case 'B':
						{
							int position = -1;
							//find the bishop...
							for(int i = 0; i < 64; i++)
							{
								if(board[i] == 3)
								{
									position = i;
									//check diagonal
									if((position-destination)<0) //if going up board
									{
										bool blocked = false;
										for(int i = destination-9; i >= position; i-=9)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Bishop found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Bishop found, move legal" << endl;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && !(board[i] == 0))
											{
												blocked = true;	
											}
										}
										blocked = false;
										for(int i = destination-7; i>=position; i-=7)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Bishop found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Bishop found, move legal" << endl;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && !(board[i] == 0))
											{
												blocked = true;	
											}
										}
									}
									else if((position-destination) == 0)
									{
										cout << "Don't make moves to the same position" << endl;
										return false;
									}
									else //if going down board
									{
										bool blocked = false;
										for(int i = destination+7; i <= position; i+=7)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Bishop found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Bishop found, move legal" << endl;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && !(board[i] == 0))
											{
												blocked = true;	
											}
										}
										blocked = false;
										for(int i = destination+9;i <= position; i+=9)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Bishop found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Bishop found, move legal" << endl;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && !(board[i] == 0))
											{
												blocked = true;	
											}
										}
									}
								}
							}
							cout << "No bishop was found that could make that move." << endl;
							return false; //no bishop was found	
						}	
				}
			}
			else
			{
				if(board[destination] < 0)
				{
					cout << "Destination has friendly piece." << endl;
					return false;
				}

				//what piece?
				switch(move[0])
				{
					//bishop
					case 'B':
						{
							int position = -1;
							//find the bishop...
							for(int i = 0; i < 64; i++)
							{
								if(board[i] == -3)
								{
									position = i;
									//check diagonal
									if((position-destination)<0) //if going up board
									{
										bool blocked = false;
										for(int i = destination-9; i >= position; i-=9)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Bishop found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Bishop found, move legal" << endl;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && !(board[i] == 0))
											{
												blocked = true;	
											}
										}
										blocked = false;
										for(int i = destination-7; i>=position; i-=7)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Bishop found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Bishop found, move legal" << endl;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && !(board[i] == 0))
											{
												blocked = true;	
											}
										}
									}
									else if((position-destination) == 0)
									{
										cout << "Don't make moves to the same position" << endl;
										return false;
									}
									else //if going down board
									{
										bool blocked = false;
										for(int i = destination+7; i <= position; i+=7)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Bishop found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Bishop found, move legal" << endl;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && !(board[i] == 0))
											{
												blocked = true;	
											}
										}
										blocked = false;
										for(int i = destination+9;i <= position; i+=9)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Bishop found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Bishop found, move legal" << endl;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && !(board[i] == 0))
											{
												blocked = true;	
											}
										}
									}
								}
							}
							cout << "No bishop was found that could make that move." << endl;
							return false; //no bishop was found
							break;	
						}
					case 'R':
						{
							int position = -1;
							//find the rook...
							for(int i = 0; i < 64; i++)
							{
								if(board[i] == -5)
								{
									position = i;
								}
							}
							cout << "No rook was found that could make that move." << endl;
							return false; //no rook was found
							break;
						}
					default:
						{
							cout << "Default case" << endl;
						}
				}
			}
			//legality exception: shows move got past all cases
			cout << "Legality exception" << endl;
			return false;
		}
};

int main()
{
	Board testBoard;
	testBoard.printBoard();
	testBoard.legalCheck("Bd3");
	testBoard.legalCheck("Bb1");
	testBoard.legalCheck("Ba6");
	testBoard.legalCheck("Bb5");

	testBoard.turn = false;
	testBoard.legalCheck("Bd7");
}