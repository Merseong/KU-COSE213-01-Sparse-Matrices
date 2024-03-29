#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#define LONGEST_COMMAND 10
#define MAX_MATRICES 50

/*
Sparse Matrices

mread: Read in a sparse matrix (scanf)
mwrite: Write out a sparse matrix (printf)
merase: Erase a sparse matrix
madd: Create the sparse matrix d = a + b
mmult: Create the sparse matrix d = a * b
mtranspose: Create the sparse matrix b = a^T
*/

typedef enum {header, entry} tag;
typedef struct _matrixNode {
	int row;
	int col;
	int value;
	tag type;
	struct _matrixNode* down; // if type is header, down is pointing first top-left term.
	union {
		struct _matrixNode* entry;
		struct _topNode* top;
	} right;
} matrixNode;

typedef struct _topNode {
	int number;
	struct _topNode* next;
	matrixNode* down;
	matrixNode* right;
} topNode;

matrixNode* matrices[MAX_MATRICES]; // array of headers of all matrix
int nextEmpty = 0;
int matCount = 0;

// functions which can use on consol UI
matrixNode* mread();
void mwrite(matrixNode* mat);
void merase(matrixNode* mat, int index);
matrixNode* madd(matrixNode* left, matrixNode* right);
matrixNode* mmult(matrixNode* left, matrixNode* right);
matrixNode* mtranspose(matrixNode* mat);

// functions which uses for other function
void ClearBuf();
matrixNode* minit(int _row, int _col);
void TermErase(matrixNode* Term);
int MakeEntry(matrixNode* mat, int _row, int _col, int _value);
int UIreader();
void UImenu(int mode);

int main()
{
	UImenu(0);
	UImenu(1);
	while (1)
	{
		if (UIreader()) break;
	}
	return 0;
}

// clear input buffer
void ClearBuf()
{
	while (getchar() != '\n');
}

// print main menu(0) or command help(1)
void UImenu(int mode)
{
	if (!mode) // main menu
	{
		printf("┌─────────────────────────────────────────────┐\n");
		printf("│           Sparse Matrix Calculator          │\n");
		printf("│     2019-1 Data Structure Assignment #2     │\n");
		printf("│         2018320205 컴퓨터학과 신대성        │\n");
		printf("└─────────────────────────────────────────────┘\n");
	}
	else if (mode) // command help
	{
		printf("┌─────  Command Help\n│\n");
		printf("│ help: Browse commands\n");
		printf("│ quit: Turn off this program\n");
		printf("│ cls: Clear screen and show main menu\n");
		printf("│ allmat: See all matrices with list\n");
		printf("│ merase: Erase a sparse matrix\n");
		printf("│ mread: Read in sparse matrix and make it\n");
		printf("│ mwrite: Write out a sparse matrix\n");
		printf("│ madd: Create the sparse matrix d = a + b\n");
		printf("│ mmult: Create the sparse matrix d = a * b\n");
		printf("│ mtranspose: Create the sparse matrix b = a^T\n");
		printf("│\n└─────  Command Help END\n");
	}
	return;
}

// scan users input and conduct that command
int UIreader()
{
	char _input[LONGEST_COMMAND + 1]; // size is come from longest command char
	printf("\n>> ");
	scanf_s("%s", _input, LONGEST_COMMAND + 1);
	ClearBuf();

	if		(!strcmp(_input, "help")) UImenu(1);
	else if (!strcmp(_input, "cls"))
	{
		system("cls");
		UImenu(0);
	}
	else if (!strcmp(_input, "quit"))
	{
		int i = 0;
		while (matCount != 0 && i < MAX_MATRICES + 1)
		{
			if (matrices[i] != NULL)
			{
				merase(matrices[i], i);
			}
			i++;
		}
		return 1;
	}
	else if (!strcmp(_input, "allmat"))
	{
		if (matCount == 0)
		{
			printf(" there are no matrix.\n");
			return 0;
		}
		for (int i = 0; i < nextEmpty; ++i)
		{
			if (matrices[i] != NULL)
			{
				printf(" index [%d], %d x %d, %d non-zero elements\n", i, matrices[i]->row, matrices[i]->col, matrices[i]->value);
			}
		}
	}
	else if (!strcmp(_input, "mread")) mread();
	else if (!strcmp(_input, "mwrite"))
	{
		int matIndex;
		printf(" Enter the index of matrix you want to view.\n\t>>> ");
		scanf("%d", &matIndex);
		ClearBuf();
		if (matrices[matIndex] != NULL) mwrite(matrices[matIndex]);
		else printf("[ERROR] there are no matrix in that index.\n");
	}
	else if (!strcmp(_input, "madd"))
	{
		int _left, _right;
		printf(" type two indexes of matrices to add.\n\t>>> ");
		scanf("%d %d", &_left, &_right);
		if (matrices[_left] != NULL && matrices[_right] != NULL)
		{
			madd(matrices[_left], matrices[_right]);
		}
		else printf("[ERROR] invalid indexes.\n");
	}
	else if (!strcmp(_input, "merase"))
	{
		int _index;
		printf(" type index of matrix which you want to erase.\n\t>>> ");
		scanf("%d", &_index);
		if (matrices[_index] != NULL)
		{
			merase(matrices[_index], _index);
		}
		else
		{
			printf("[ERROR] there are no matrix in that index.\n");
		}
	}
	else if (!strcmp(_input, "mmult"))
	{
		int _left, _right;
		printf(" type two indexes of matrices to multiply.\n\t>>> ");
		scanf("%d %d", &_left, &_right);
		if (matrices[_left] != NULL && matrices[_right] != NULL)
		{
			mmult(matrices[_left], matrices[_right]);
		}
		else printf("[ERROR] invalid indexes.\n");
	}
	else if (!strcmp(_input, "mtranspose"))
	{
		int _mat;
		printf(" type index of matrix to transpose.\n\t>>> ");
		scanf("%d", &_mat);
		if (matrices[_mat] != NULL)
		{
			mtranspose(matrices[_mat]);
		}
		else printf("[ERROR] invalid indexes.\n");
	}
	else printf("[ERROR] wrong input, try again.\n");

	return 0;
}

// make new empty matrix which have _rowSize and _colSize
matrixNode* minit(int _rowSize, int _colSize)
{
	if (_rowSize < 1 || _colSize < 1) // error when invalid size
	{
		printf("[ERROR] row and column must be bigger than 0.\n");
		return NULL;
	}

	matrixNode* out = (matrixNode*)malloc(sizeof(matrixNode));

	// set inital value of new matrix
	out->type = header;
	out->row = _rowSize;
	out->col = _colSize;
	out->value = 0;
	out->down = NULL;

	int topCount = max(_rowSize, _colSize);
	topNode* top = (topNode*)malloc(sizeof(topNode)); // make tops and link them
	out->right.top = top;
	top->down = NULL;
	top->right = NULL;
	top->number = 0;
	for (int i = 0; i < topCount - 1; ++i)
	{
		topNode* newtop = (topNode*)malloc(sizeof(topNode));
		newtop->down = NULL;
		newtop->right = NULL;
		newtop->number = i + 1;

		top->next = newtop;
		top = newtop;
	}
	top->next = NULL; // last top's next will be NULL

	matrices[nextEmpty++] = out;
	matCount++;

	return out;
}

// free the matrix
void merase(matrixNode* mat, int index)
{
	// erase at array matrices
	matrices[index] = NULL;
	if (index == --matCount) nextEmpty--;

	for (topNode* top = mat->right.top; top != NULL;)
	{
		TermErase(top->right); // free terms in top
		topNode* tmp_top = top->next;
		free(top); // free top
		top = tmp_top;
	}

	// free header
	free(mat);

	printf(" done erasing matrix index [%d]\n", index);
}

// recursively erases terms linked with row
void TermErase(matrixNode* term)
{
	if (term != NULL)
	{
		TermErase(term->right.entry);
		free(term);
	}
}

// read in sparse matrix and make it
matrixNode* mread()
{
	int _row, _col, _ele;
	printf(" Enter the size of rows, columns and the number of elements(nonzero terms)\n\t>>> ");
	scanf("%d %d %d", &_row, &_col, &_ele);
	ClearBuf();

	// error handle
	if (_ele <= 0)
	{
		printf("[ERROR] input about size must be positive natural number.\n");
		return NULL;
	}
	else if (_ele > _row * _col)
	{
		printf("[ERROR] number of non-zero element is bigger than number of row * column.\n");
		return NULL;
	}
	
	matrixNode* out = minit(_row, _col);
	if (out == NULL) return NULL;
	out->value = _ele;

	printf(" Enter the term's row, column and value.(row and column is zero-based)\n");
	for (int i = 0; i < _ele; ++i)
	{
		int t_row, t_col, t_val; // term's row, col, value
		printf("\t>>> ");
		scanf("%d %d %d", &t_row, &t_col, &t_val);
		ClearBuf();
		if (MakeEntry(out, t_row, t_col, t_val))
		{
			--i; continue;
		}
	}

	mwrite(out);

	return out;
}

// print matrix
void mwrite(matrixNode* mat)
{
	matrixNode* currentNode = mat->down;
	topNode* currentTop = mat->right.top;

	// find index of matrix
	int index = -1;
	for (int i = 0; i < nextEmpty; i++)
	{
		if (matrices[i] == mat)
		{
			index = i;
			break;
		}
	}
	printf("\n index [%d], %d x %d, %d non-zero elements\n\n", index, mat->row, mat->col, mat->value);

	for (int rowCnt = 0; rowCnt < mat->row; ++rowCnt)
	{
		for (int colCnt = 0; colCnt < mat->col; ++colCnt)
		{
			if (currentNode != NULL && currentNode->row == rowCnt && currentNode->col == colCnt)
			{
				printf(" | row: %2d | column: %2d | value: %2d |\n", currentNode->row, currentNode->col, currentNode->value);
				currentNode = currentNode->right.entry;
			}
		}
		if (currentTop->next != NULL) currentTop = currentTop->next;
		if (currentNode == NULL) currentNode = currentTop->right;
	}
}

// make new Entry and put this in to matrix.
int MakeEntry(matrixNode* mat, int _row, int _col, int _value)
{
	// error when row and col is out of range
	if (mat->row <= _row || mat->col <= _col)
	{
		printf("[ERROR] row and col must be smaller than matrix's rowSize and colSize. try again.\n");
		return -1;
	}
	else if (_row < 0 || _col < 0)
	{
		printf("[ERROR] row and column must be positive. try again.\n");
		return -1;
	}
	// error when value is zero
	else if (_value == 0)
	{
		printf("[ERROR] your input is zero term. type non-zero term again.\n");
		return -1;
	}

	matrixNode* newEntry = (matrixNode*)malloc(sizeof(matrixNode));
	
	newEntry->type = entry;
	newEntry->row = _row;
	newEntry->col = _col;
	newEntry->value = _value;

	topNode* rowTop = mat->right.top;
	topNode* colTop = mat->right.top;
	while (rowTop->next != NULL && rowTop->number != _row) rowTop = rowTop->next;
	while (colTop->next != NULL && colTop->number != _col) colTop = colTop->next;
	
	// when this is first term
	if (mat->down == NULL)
	{
		mat->down = newEntry;
		newEntry->down = NULL;
		newEntry->right.entry = NULL;
		rowTop->right = newEntry;
		colTop->down = newEntry;
	}
	else
	{
		newEntry->down = NULL;
		newEntry->right.entry = NULL;

		matrixNode* rowBefore = NULL;
		matrixNode* rowNext = rowTop->right;
		matrixNode* colBefore = NULL;
		matrixNode* colNext = colTop->down;

		while (newEntry->right.entry == NULL) // find proper position of row
		{
			if (rowNext != NULL)
			{
				if (rowNext->col > _col)
				{
					newEntry->right.entry = rowNext;
					if (rowBefore != NULL) rowBefore->right.entry = newEntry;
					else rowTop->right = newEntry; // when this term is first right term of that top node
				}
				else if (rowNext->col == _col) // same position
				{
					printf("[ERROR] there are already other term. try another.\n");
					free(newEntry);
					return -1;
				}
				else // rowNext->col < _col
				{
					rowBefore = rowNext;
					rowNext = rowNext->right.entry;
				}
			}
			else // first term of that empty top node or last term
			{
				if (rowBefore != NULL) rowBefore->right.entry = newEntry; // last term of top node
				else rowTop->right = newEntry; // first term of that empty top node
				break;
			}
		}

		while (newEntry->down == NULL) // find proper position of column
		{
			if (colNext != NULL)
			{
				if (colNext->row > _row)
				{
					newEntry->down = colNext;
					if (colBefore != NULL) colBefore->down = newEntry;
					else colTop->down = newEntry;
				} // there are no same case
				else // colNext->row < _row
				{
					colBefore = colNext;
					colNext = colNext->down;
				}
			}
			else
			{
				if (colBefore != NULL) colBefore->down = newEntry;
				else colTop->down = newEntry;
				break;
			}
		}

		if (mat->down->row > _row) mat->down = newEntry;
		else if (mat->down->row == _row && mat->down->col > _col) mat->down = newEntry;
	}
	return 0;
}

// add two matrices and make new matrix
matrixNode* madd(matrixNode* left, matrixNode* right)
{
	if ((left->col != right->col) || (left->row != right->row))
	{
		printf("[ERROR] two inputs do not have same size of row and column.\n");
		return NULL;
	}

	matrixNode* out = minit(left->row, left->col);

	topNode* currentTopL = left->right.top;
	topNode* currentTopR = right->right.top;
	matrixNode* currentNodeL = currentTopL->right;
	matrixNode* currentNodeR = currentTopR->right;
	while (currentTopL != NULL)
	{
		if (currentNodeL != NULL && currentNodeR != NULL)
		{
			if (currentNodeL->col < currentNodeR->col)
			{
				MakeEntry(out, currentNodeL->row, currentNodeL->col, currentNodeL->value);
				currentNodeL = currentNodeL->right.entry;
				out->value++;
			}
			else if (currentNodeL->col == currentNodeR->col)
			{
				int _val = currentNodeL->value + currentNodeR->value;
				if (_val != 0)
				{
					MakeEntry(out, currentNodeL->row, currentNodeL->col, _val);
					out->value++;
				}
				currentNodeL = currentNodeL->right.entry;
				currentNodeR = currentNodeR->right.entry;
			}
			else // NodeL->col > NodeR->col
			{
				MakeEntry(out, currentNodeR->row, currentNodeR->col, currentNodeR->value);
				currentNodeR = currentNodeR->right.entry;
				out->value++;
			}
		}
		else if (currentNodeL != NULL)
		{
			MakeEntry(out, currentNodeL->row, currentNodeL->col, currentNodeL->value);
			currentNodeL = currentNodeL->right.entry;
			out->value++;
		}
		else if (currentNodeR != NULL)
		{
			MakeEntry(out, currentNodeR->row, currentNodeR->col, currentNodeR->value);
			currentNodeR = currentNodeR->right.entry;
			out->value++;
		}
		else // both node is NULL
		{
			currentTopL = currentTopL->next;
			currentTopR = currentTopR->next;
			if (currentTopL != NULL) currentNodeL = currentTopL->right;
			if (currentTopR != NULL) currentNodeR = currentTopR->right;
		}
	}

	mwrite(out);
	return out;
}

// multiply two matrices and make new matrix
matrixNode* mmult(matrixNode* left, matrixNode* right)
{
	if (left->col != right->row) // error detect
	{
		printf("[ERROR] when multiply two matrices, left-side's column and right-side's row must be same.\n");
		return NULL;
	}

	matrixNode* out = minit(left->row, right->col);

	topNode* currentTopL = left->right.top; // row
	topNode* currentTopR = right->right.top; // column
	matrixNode* currentNodeL = currentTopL->right;
	matrixNode* currentNodeR = currentTopR->down;
	int sum = 0;
	while (currentTopL != NULL)
	{
		while (currentTopR != NULL)
		{
			if (currentNodeL != NULL && currentNodeR != NULL)
			{
				if (currentNodeL->col < currentNodeR->row)
				{
					currentNodeL = currentNodeL->right.entry;
				}
				else if (currentNodeL->col == currentNodeR->row)
				{
					sum += currentNodeL->value * currentNodeR->value;
					currentNodeL = currentNodeL->right.entry;
					currentNodeR = currentNodeR->down;
				}
				else // NodeL->col > NodeR->row
				{
					currentNodeR = currentNodeR->down;
				}
			}
			else // one of nodes is NULL
			{
				if (sum != 0)
				{
					MakeEntry(out, currentTopL->number, currentTopR->number, sum);
					out->value++;
				}
				sum = 0;
				
				currentTopR = currentTopR->next;
				if (currentTopL != NULL) currentNodeL = currentTopL->right;
				if (currentTopR != NULL) currentNodeR = currentTopR->down;
			}
		}
		currentTopL = currentTopL->next;
		currentTopR = right->right.top;
		if (currentTopL != NULL) currentNodeL = currentTopL->right;
		if (currentTopR != NULL) currentNodeR = currentTopR->down;
	}

	mwrite(out);
	return out;
}

// return transposed matrix
matrixNode* mtranspose(matrixNode* mat)
{
	matrixNode* out = minit(mat->col, mat->row);
	out->value = mat->value;

	topNode* currentTop = mat->right.top;
	matrixNode* currentNode = currentTop->right;
	while (currentTop != NULL)
	{
		if (currentNode != NULL)
		{
			MakeEntry(out, currentNode->col, currentNode->row, currentNode->value);
			currentNode = currentNode->right.entry;
		}
		else
		{
			currentTop = currentTop->next;
			if (currentTop != NULL) currentNode = currentTop->right;
		}
	}

	mwrite(out);
	return out;
}