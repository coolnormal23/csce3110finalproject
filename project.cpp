#include <iostream>
#include <vector>
#include <cmath>
#include <string>
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
			2,   2,  2,  2,  2,  2,  2,  2,
			0,   0,  0,  0,  0,  0,  0,  0,
			0,   0,  0,  0,  0,  0,  0,  0,
			0,   0,  0,  0,  0,  0,  0,  0,
			0,   0,  0,  0,  0,  0,  0,  0,
			-2, -2, -2, -2, -2, -2, -2, -2,
			-5, -4, -3, -6, -1, -3, -4, -5
		};

		int eval;
		bool check;
		bool turn;
		vector<Board> children;
		int nextboard;
		int pieceLocation = -1;
		int pieceDestination = -1;

		Board()
		{
			turn = true;
			eval = 0;
			check = false;
		};

		void printBoard()
		{
			if(turn)
			{
				cout << "White's turn" << endl;
			}
			else
			{
				cout << "Black's turn" << endl;
			}

			evaluate();
			cout << "Evaluation: " << eval << endl;

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
			pieceDestination = destination;

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
													pieceLocation = i;
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
													pieceLocation = i;
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
								if(board[i] == 5)
								{
									position = i;
									if((destination%8) == (position%8)) //if on some file
									{
										if((destination-position)>0)
										{
											bool blocked = false;
											for(int i = destination; i >= position; i-=8)
											{
												if(i == position)
												{
													if(blocked)
													{
														cout << "Rook found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Rook found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && !(board[i] == 0))
												{
													blocked = true;
												}
											}
										}
										else if((destination-position)<0)
										{
											bool blocked = false;
											for(int i = destination; i <= position; i+=8)
											{
												if(i == position)
												{
													if(blocked)
													{
														cout << "Rook found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Rook found, move legal" << endl;
														pieceLocation = i;
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
									else //same rank check
									{
										bool blocked = false;
										for(int i = destination; i <= (destination+(8-(destination%8))); i++)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Rook found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Rook found, move legal" << endl;
													pieceLocation = i;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && !(board[i] == 0))
											{
												blocked = true;
											}
										}
										blocked = false;
										for(int i = destination; i >= (destination-(destination%8)); i--)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Rook found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Rook found, move legal" << endl;
													pieceLocation = i;
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
							cout << "No rook was found that could make that move." << endl;
							return false; //no rook was found
							break;
						}
					case 'Q':
						{
							int position = -1;
							//find with "rook like movements" in cardinal directions
							for(int i = 0; i < 64; i++)
							{
								if(board[i] == 6)
								{
									position = i;
									if(destination == position)
									{
										cout << "Don't make moves to the same square." << endl;
										return false;
									}
									else if((destination%8) == (position%8)) //if on some file
									{
										if((destination-position)>0)
										{
											bool blocked = false;
											for(int i = destination; i >= position; i-=8)
											{
												if(i == position)
												{
													if(blocked)
													{
														cout << "Queen found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Queen found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
												{
													blocked = true;
												}
											}
										}
										else if((destination-position)<0)
										{
											bool blocked = false;
											for(int i = destination; i <= position; i+=8)
											{
												if(i == position)
												{
													if(blocked)
													{
														cout << "Queen found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Queen found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
												{
													blocked = true;
												}
											}
										}
									}
									else //same rank check
									{
										bool blocked = false;
										for(int i = destination; i <= (destination+(8-(destination%8))); i++)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Queen found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Queen found, move legal" << endl;
													pieceLocation = i;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
											{
												blocked = true;
											}
										}
										blocked = false;
										for(int i = destination; i >= (destination-(destination%8)); i--)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Queen found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Queen found, move legal" << endl;
													pieceLocation = i;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
											{
												blocked = true;
											}
										}
									}
								}
								for(int i = 0; i < 64; i++)
								{
									if(board[i] == 6)
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
														cout << "Queen found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Queen found, move legal" << endl;
														pieceLocation = i;
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
														cout << "Queen found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Queen found, move legal" << endl;
														pieceLocation = i;
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
														cout << "Queen found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Queen found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && i!= destination && !(board[i] == 0))
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
														cout << "Queen found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Queen found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && i!= destination && !(board[i] == 0))
												{
													blocked = true;	
												}
											}
										}
									}
								}
							}
							cout << "No valid queen move found." << endl;
							return false;
							break;
						}
					case 'N':
						{
							//using knight directions compass: www.chessprogramming.org/Direction#Knight_Directions

							int position = -1;
							bool noNoWe, noNoEa, noWeWe, noEaEa, soWeWe, soEaEa, soSoWe, soSoEa;
							noNoWe = noNoEa = noWeWe = noEaEa = soWeWe = soEaEa = soSoWe = soSoEa = true;

							//rule out directions
							if(destination >= 48)
							{
								noNoWe = noNoEa = false;
							}
							if((destination%8) < 2)
							{
								noWeWe = soWeWe = false;
							}
							if((destination%8) > 5)
							{
								noEaEa = soEaEa = false;
							}
							if(destination <= 15)
							{
								soSoWe = soSoEa = false;
							}
							if(destination >= 56)
							{
								noWeWe = noEaEa = false;
							}
							if(destination <= 7)
							{
								soWeWe = soEaEa = false;
							}
							if((destination%8) == 0)
							{
								noNoWe = soSoWe = false;
							}
							if((destination%8) == 7)
							{
								noNoEa = soSoEa = false;
							}

							if(noNoWe)
							{
								if(board[destination-15] == 4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination-15);
									return true;
								}
							}
							if(noNoEa)
							{
								if(board[destination-17] == 4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination-17);
									return true;
								}
							}
							if(noWeWe)
							{
								if(board[destination-6] == 4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination-6);
									return true;
								}
							}
							if(noEaEa)
							{
								if(board[destination-10] == 4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination-10);
									return true;
								}
							}
							if(soWeWe)
							{
								if(board[destination+10] == 4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination+10);
									return true;
								}
							}
							if(soEaEa)
							{
								if(board[destination+6] == 4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination+6);
									return true;
								}
							}
							if(soSoWe)
							{
								if(board[destination+17] == 4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination+17);
									return true;
								}
							}
							if(soSoEa)
							{
								if(board[destination+15] == 4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination+15);
									return true;
								}
							}

							cout << "Knight not found." << endl;
							return false;
							break;
						}
					case 'K':
						{
							int position = -1;
							if(board[(destination-1)] == 1)
							{
								position = destination-1;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination-7)] == 1)
							{
								position = destination-7;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination-8)] == 1)
							{
								position = destination-8;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination-9)] == 1)
							{
								position = destination-9;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination+1)] == 1)
							{
								position = destination+1;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination+7)] == 1)
							{
								position = destination+7;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination+8)] == 1)
							{
								position = destination+8;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination+9)] == 1)
							{
								position = destination+9;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}

							cout << "King not found" << endl;
							return false;
							break;
						}
					default:
						{
							//pawn moves
							//first, check for pawn captures
							if(board[destination] != 0)
							{
								if((destination%8) == 0)
								{
									if(board[(destination-7)] == 2)
									{
										cout << "Capturing pawn found" << endl;
										pieceLocation = destination-7;
										return true;
									}
								}
								else if((destination%8) == 7)
								{
									if(board[(destination-9)] == 2)
									{
										cout << "Capturing pawn found" << endl;
										pieceLocation = destination-7;
										return true;
									}
								}
								else
								{
									//check specified file first
									if(file != -1 && file >= 0 && file <= 7)
									{
										if(file < (destination%8))
										{
											if(board[destination-9] == 2)
											{
												cout << "Capturing pawn found" << endl;
												pieceLocation = destination-9;
												return true;
											}
										}
										else if(file > (destination%8))
										{
											if(board[destination-7] == 2)
											{
												cout << "Capturing pawn found" << endl;
												pieceLocation = destination-7;
												return true;
											}
										}
									}
									else
									{
										if(board[destination-9] == 2)
										{
											cout << "Capturing pawn found" << endl;
											pieceLocation = destination-9;
											return true;
										}
										if(board[destination-7] == 2)
										{
											cout << "Capturing pawn found" << endl;
											pieceLocation = destination-7;
											return true;
										}
									}
								}
							}
							else
							{
								if(board[destination-8] == 2)
								{
									cout << "Moving pawn found" << endl;
									pieceLocation = destination-8;
									return true;
								}
								else if(board[destination-8] != 0 && board[destination-16] == 2)
								{
									cout << "Pawn found, but blocked from double moving" << endl;
									return false;
								}
								else if(board[destination-8] == 0 && board[destination-16] == 2 && destination >= 24 && destination <= 31)
								{
									cout << "Pawn found, double move" << endl;
									pieceLocation = destination-16;
									return true;
								}
							}
							cout << "No eligable pawn found" << endl;
							return false;
						}
				}
			}
			else //if blacks turn
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
													pieceLocation = i;
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
													pieceLocation = i;
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
													pieceLocation = i;
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
													pieceLocation = i;
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
									if((destination%8) == (position%8)) //if on some file
									{
										if((destination-position)>0)
										{
											bool blocked = false;
											for(int i = destination; i >= position; i-=8)
											{
												if(i == position)
												{
													if(blocked)
													{
														cout << "Rook found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Rook found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
												{
													blocked = true;
												}
											}
										}
										else if((destination-position)<0)
										{
											bool blocked = false;
											for(int i = destination; i <= position; i+=8)
											{
												if(i == position)
												{
													if(blocked)
													{
														cout << "Rook found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Rook found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
												{
													blocked = true;
												}
											}
										}
									}
									else //same rank check
									{
										bool blocked = false;
										for(int i = destination; i <= (destination+(8-(destination%8))); i++)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Rook found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Rook found, move legal" << endl;
													pieceLocation = i;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
											{
												blocked = true;
											}
										}
										blocked = false;
										for(int i = destination; i >= (destination-(destination%8)); i--)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Rook found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Rook found, move legal" << endl;
													pieceLocation = i;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
											{
												blocked = true;
											}
										}
									}
								}
							}
							cout << "No rook was found that could make that move." << endl;
							return false; //no rook was found
							break;
						}
					case 'Q':
						{
							int position = -1;
							//find with "rook like movements" in cardinal directions
							for(int i = 0; i < 64; i++)
							{
								if(board[i] == -6)
								{
									position = i;
									if(destination == position)
									{
										cout << "Don't make moves to the same square." << endl;
										return false;
									}
									else if((destination%8) == (position%8)) //if on some file
									{
										if((destination-position)>0)
										{
											bool blocked = false;
											for(int i = destination; i >= position; i-=8)
											{
												if(i == position)
												{
													if(blocked)
													{
														cout << "Queen found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Queen found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && !(board[i] == 0))
												{
													blocked = true;
												}
											}
										}
										else if((destination-position)<0)
										{
											bool blocked = false;
											for(int i = destination; i <= position; i+=8)
											{
												if(i == position)
												{
													if(blocked)
													{
														cout << "Queen found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Queen found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
												{
													blocked = true;
												}
											}
										}
									}
									else //same rank check
									{
										bool blocked = false;
										for(int i = destination; i <= (destination+(8-(destination%8))); i++)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Queen found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Queen found, move legal" << endl;
													pieceLocation = i;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && !(board[i] == 0))
											{
												blocked = true;
											}
										}
										blocked = false;
										for(int i = destination; i >= (destination-(destination%8)); i--)
										{
											if(i == position)
											{
												if(blocked)
												{
													cout << "Queen found, but path blocked" << endl;
													return false;
												}
												else
												{
													cout << "Queen found, move legal" << endl;
													pieceLocation = i;
													return true;
												}
											}
											else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
											{
												blocked = true;
											}
										}
									}
								}
								for(int i = 0; i < 64; i++)
								{
									if(board[i] == -6)
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
														cout << "Queen found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Queen found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
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
														cout << "Queen found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Queen found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
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
														cout << "Queen found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Queen found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
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
														cout << "Queen found, but path blocked" << endl;
														return false;
													}
													else
													{
														cout << "Queen found, move legal" << endl;
														pieceLocation = i;
														return true;
													}
												}
												else if(i >= 0 && i <= 63 && i != destination && !(board[i] == 0))
												{
													blocked = true;	
												}
											}
										}
									}
								}
							}
							cout << "No valid queen move found." << endl;
							return false;
							break;
						}
					case 'N':
						{
							//using knight directions compass: www.chessprogramming.org/Direction#Knight_Directions

							int position = -1;
							bool noNoWe, noNoEa, noWeWe, noEaEa, soWeWe, soEaEa, soSoWe, soSoEa;
							noNoWe = noNoEa = noWeWe = noEaEa = soWeWe = soEaEa = soSoWe = soSoEa = true;

							//rule out directions
							if(destination >= 48)
							{
								noNoWe = noNoEa = false;
							}
							if((destination%8) < 2)
							{
								noWeWe = soWeWe = false;
							}
							if((destination%8) > 5)
							{
								noEaEa = soEaEa = false;
							}
							if(destination <= 15)
							{
								soSoWe = soSoEa = false;
							}
							if(destination >= 56)
							{
								noWeWe = noEaEa = false;
							}
							if(destination <= 7)
							{
								soWeWe = soEaEa = false;
							}
							if((destination%8) == 0)
							{
								noNoWe = soSoWe = false;
							}
							if((destination%8) == 7)
							{
								noNoEa = soSoEa = false;
							}

							if(noNoWe)
							{
								if(board[destination-15] == -4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination-15);
									return true;
								}
							}
							if(noNoEa)
							{
								if(board[destination-17] == -4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination-17);
									return true;
								}
							}
							if(noWeWe)
							{
								if(board[destination-6] == -4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination-6);
									return true;
								}
							}
							if(noEaEa)
							{
								if(board[destination-10] == -4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination-10);
									return true;
								}
							}
							if(soWeWe)
							{
								if(board[destination+10] == -4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination+10);
									return true;
								}
							}
							if(soEaEa)
							{
								if(board[destination+6] == -4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination+6);
									return true;
								}
							}
							if(soSoWe)
							{
								if(board[destination+17] == -4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination+17);
									return true;
								}
							}
							if(soSoEa)
							{
								if(board[destination+15] == -4)
								{
									cout << "Knight found" << endl;
									pieceLocation = (destination+15);
									return true;
								}
							}

							cout << "Knight not found." << endl;
							return false;
							break;
						}
					case 'K':
						{
							int position = -1;
							if(board[(destination-1)] == -1)
							{
								position = destination-1;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination-7)] == -1)
							{
								position = destination-7;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination-8)] == -1)
							{
								position = destination-8;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination-9)] == -1)
							{
								position = destination-9;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination+1)] == -1)
							{
								position = destination+1;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination+7)] == -1)
							{
								position = destination+7;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination+8)] == -1)
							{
								position = destination+8;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}
							else if(board[(destination+9)] == -1)
							{
								position = destination+9;
								pieceLocation = position;
								cout << "King found" << endl;
								return true;
							}

							cout << "King not found" << endl;
							return false;
							break;
						}
					default:
						{
							//pawn moves
							//first, check for pawn captures
							if(board[destination] != 0)
							{
								if((destination%8) == 0)
								{
									if(board[(destination+7)] == -2)
									{
										cout << "Capturing pawn found" << endl;
										pieceLocation = destination+7;
										return true;
									}
								}
								else if((destination%8) == 7)
								{
									if(board[(destination+9)] == -2)
									{
										cout << "Capturing pawn found" << endl;
										pieceLocation = destination+7;
										return true;
									}
								}
								else
								{
									//check specified file first
									if(file != -1 && file >= 0 && file <= 7)
									{
										if(file < (destination%8))
										{
											if(board[destination+9] == -2)
											{
												cout << "Capturing pawn found" << endl;
												pieceLocation = destination+9;
												return true;
											}
										}
										else if(file > (destination%8))
										{
											if(board[destination+7] == -2)
											{
												cout << "Capturing pawn found" << endl;
												pieceLocation = destination+7;
												return true;
											}
										}
									}
									else
									{
										if(board[destination+9] == -2)
										{
											cout << "Capturing pawn found" << endl;
											pieceLocation = destination+9;
											return true;
										}
										if(board[destination+7] == -2)
										{
											cout << "Capturing pawn found" << endl;
											pieceLocation = destination+7;
											return true;
										}
									}
								}
							}
							else
							{
								if(board[destination+8] == -2)
								{
									cout << "Moving pawn found" << endl;
									pieceLocation = destination+8;
									return true;
								}
								else if(board[destination+8] != 0 && board[destination+16] == -2)
								{
									cout << "Pawn found, but blocked from double moving" << endl;
									return false;
								}
								else if(board[destination+8] == 0 && board[destination+16] == -2 && destination >= 32 && destination <= 39)
								{
									cout << "Pawn found, double move" << endl;
									pieceLocation = destination+16;
									return true;
								}
							}
							cout << "No eligable pawn found" << endl;
							return false;
						}

				}
			}

			//legality exception: shows move got past all cases
			cout << "Legality exception" << endl;
			return false;
		}

		bool playMove(Board& movingBoard, string move)
		{
			if(legalCheck(move))
			{
				if(pieceLocation != -1 && pieceDestination != -1)
				{
					cout << "Playing " << move << "." << endl;
					movingBoard.board[pieceDestination] = movingBoard.board[pieceLocation];
					movingBoard.board[pieceLocation] = 0;

					//win conditon check
					bool whiteWin = true;
					for(int i = 0; i < 64; i++)
					{
						if(movingBoard.board[i] == -1)
						{
							whiteWin = false;
						}
					}
					if(whiteWin)
					{
						cout << "White wins." << endl;
						exit(0);
					}

					bool blackWin = true;
					for(int i = 0; i < 64; i++)
					{
						if(movingBoard.board[i] == 1)
						{
							blackWin = false;
						}
					}
					if(blackWin)
					{
						cout << "Black wins!!!!!!!!!!!!!!!!!!!!!!!" << endl;
						exit(0);
					}


					movingBoard.turn = !movingBoard.turn;
					return true;
				}
				else
				{
					cout << "Location or destination not set." << endl;
					return false;
				}
			}
			else
			{
				cout << "Move not legal" << endl;
				return false;
			}
		}
};

int main()
{
	Board testBoard;
	string input;
	while(input != "quit")
	{
		testBoard.printBoard();
		cout << "Make move: ";
		getline(cin, input);
		if(testBoard.legalCheck(input))
		{
			Board newBoard = testBoard;
			if(newBoard.playMove(newBoard, input))
			{
				testBoard.playMove(testBoard, input);
			}
		}
	}
}
