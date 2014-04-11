
#line 1 "parse_assignment.rl"
#include "parse_assignment.h"
#include <stdlib.h>
#include <stdio.h>



#line 56 "parse_assignment.rl"


int parse_assignment(char *start, char *end, char **name, struct valout_t *valout) {
	
	/* Write data for machine, suppress unused variables error and first_final */
	
#line 17 "parse_assignment.c"
static const char _assignment_actions[] = {
	0, 1, 0, 1, 1, 1, 3, 1, 
	4, 1, 5, 2, 2, 3
};

static const char _assignment_key_offsets[] = {
	0, 0, 4, 12, 16, 26, 35, 46, 
	48, 54, 54, 56, 59
};

static const char _assignment_trans_keys[] = {
	65, 90, 97, 122, 32, 61, 9, 13, 
	65, 90, 97, 122, 32, 61, 9, 13, 
	32, 34, 39, 43, 45, 48, 9, 13, 
	49, 57, 32, 9, 13, 48, 57, 65, 
	90, 97, 122, 32, 34, 39, 9, 13, 
	48, 57, 65, 90, 97, 122, 48, 57, 
	48, 57, 65, 70, 97, 102, 48, 57, 
	120, 48, 57, 48, 57, 65, 70, 97, 
	102, 0
};

static const char _assignment_single_lengths[] = {
	0, 0, 2, 2, 6, 1, 3, 0, 
	0, 0, 0, 1, 0
};

static const char _assignment_range_lengths[] = {
	0, 2, 3, 1, 2, 4, 4, 1, 
	3, 0, 1, 1, 3
};

static const char _assignment_index_offsets[] = {
	0, 0, 3, 9, 13, 22, 28, 36, 
	38, 42, 43, 45, 48
};

static const char _assignment_indicies[] = {
	0, 0, 1, 2, 3, 2, 4, 4, 
	1, 5, 6, 5, 1, 6, 7, 7, 
	8, 8, 9, 6, 10, 1, 11, 11, 
	11, 11, 11, 1, 12, 13, 13, 12, 
	12, 12, 12, 1, 14, 1, 15, 15, 
	15, 1, 1, 14, 1, 16, 14, 1, 
	15, 15, 15, 1, 0
};

static const char _assignment_trans_targs[] = {
	2, 0, 3, 4, 2, 3, 4, 5, 
	7, 11, 10, 6, 6, 9, 10, 12, 
	8
};

static const char _assignment_trans_actions[] = {
	1, 0, 3, 3, 0, 0, 0, 0, 
	11, 11, 11, 7, 0, 9, 5, 5, 
	5
};

static const int assignment_start = 1;

static const int assignment_en_assignment = 1;


#line 63 "parse_assignment.rl"


	/* Define variables for ragel */
	int cs;
	char *p = start;
	char *pe = end;
	char *eof = pe;

	/* State end variables */
	char *value_start = NULL;
	char *value_end = NULL;
	char *name_start = NULL;
	char *name_end = NULL;

	/* Clear name type for error detection */
	*name = NULL;

	/* Clear value type for error detection */
	valout->type = VAL_NONE;

	/* Execute ragel machine, */
	/* WARNING: may return inside this if errors occur, see machine */
	
#line 106 "parse_assignment.c"
	{
	cs = assignment_start;
	}

#line 111 "parse_assignment.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _assignment_trans_keys + _assignment_key_offsets[cs];
	_trans = _assignment_index_offsets[cs];

	_klen = _assignment_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _assignment_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _assignment_indicies[_trans];
	cs = _assignment_trans_targs[_trans];

	if ( _assignment_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _assignment_actions + _assignment_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 9 "parse_assignment.rl"
	{
		name_start 	= p;
	}
	break;
	case 1:
#line 13 "parse_assignment.rl"
	{
		*name       = name_start;
		name_end 	= p;
	}
	break;
	case 2:
#line 18 "parse_assignment.rl"
	{
		value_start = p;
	}
	break;
	case 3:
#line 22 "parse_assignment.rl"
	{
		value_end 	= p;
		valout->type= VAL_INT;
	}
	break;
	case 4:
#line 27 "parse_assignment.rl"
	{
		value_start = p;
	}
	break;
	case 5:
#line 31 "parse_assignment.rl"
	{
		valout->string 	= value_start;	
		value_end 		= p;
		valout->type 	= VAL_STR;
	}
	break;
#line 225 "parse_assignment.c"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 88 "parse_assignment.rl"


	/* Null terminate strings at end of machine. */
	if (*name) {
		*name_end = '\0';
	}

	if (valout->type == VAL_STR) {
		*value_end = '\0';
	}
	else if (valout->type == VAL_INT) {
		value_end = '\0';
		/*If base = 0, strtol automatically converts hex or int */
		valout->integer = (int)strtol(value_start, NULL, 0); 
	} else { /* Unknown type */
		return -1;
	}

	return 0; /* Success */

	/* HACK: SUPPRESS COMPILER WARNINGS, SHOULD NEVER RUN AS AFTER RETURN */
	eof = pe;
	cs = assignment_en_assignment; /* Generated by ragel but unused */
}
