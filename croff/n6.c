/*
 * n6.c - Width functions, sizes and fonts for troff/nroff
 * 
 * This module handles character width calculations, font management,
 * point size changes, and motion commands in the troff/nroff system.
 */

#include "tdef.h"
#include "t.h"
#define NROFF 1
#include "tw.h"

/* External variables - character and formatting state */
extern int eschar;      /* escape character */
extern int widthp;      /* previous width */
extern int ohc;         /* overhang character */
extern int xfont;       /* extended font */
extern int smnt;        /* special mount */
extern int setwdf;      /* set width flag */
extern char trtab[];    /* character translation table */
extern int chbits;      /* character bits encoding */
extern int nonumb;      /* no number flag */
extern int noscale;     /* no scaling flag */
extern int font;        /* current font */
extern int font1;       /* previous font */
extern int pts;         /* point size */
extern int sps;         /* space size */
extern int nlflg;       /* newline flag */
extern int nform;       /* no format flag */
extern int dfact;       /* default factor */
extern int dfactd;      /* default factor denominator */
extern int lss;         /* line spacing size */
extern int lss1;        /* previous line spacing size */
extern int vflag;       /* vertical flag */
extern int ch0;         /* character 0 */
extern int level;       /* current level */
extern int ch;          /* current character */
extern int res;         /* resolution */
extern int xxx;         /* unused variable */

/* External function declarations */
extern int getrq(void); /* get request function */
extern int getch(void); /* get character function */
extern void skip(void); /* skip whitespace function */
extern void setfont(int parse_arg); /* set font function */
extern int getstr(void); /* get string function */
extern void setn1(int value); /* set numeric register function */
extern int tatoi(void); /* translate and convert to integer function */
extern int makem(int value); /* make motion function */
extern int quant(int value, int direction); /* quantize motion function */

/* Default font labels - R(oman), I(talic), B(old), S(pecial) */
int fontlab[] = {'R', 'I', 'B', 'S', 0};

/* Function prototypes for internal functions */
static int validate_character(int c);
static int calculate_motion_width(int c);
static int get_character_width(int c);
static int mot(void);

/*
 * Return the width of character 'c'.
 * 
 * This function calculates the width of a character, handling:
 * - Motion commands (horizontal and vertical)
 * - Special characters and escape sequences
 * - Character translation and font metrics
 * 
 * Parameters:
 *   c - character code, possibly with motion or formatting bits
 * 
 * Returns:
 *   Width in device units, or 0 for zero-width characters
 */
int width(int c)
{
    int character_code;
    int width_result;
    int translated_char;

    character_code = c;
    width_result = 0;

    /* Handle motion commands */
    if (character_code & MOT) {
        return calculate_motion_width(character_code);
    }

    /* Extract base character code */
    translated_char = character_code & CMASK;

    /* Handle backspace (010 octal = 8 decimal) */
    if (translated_char == 010) {
        width_result = -widthp;
        goto return_width;
    }

    /* Handle escape character substitution */
    if (translated_char == PRESC) {
        translated_char = eschar;
    }

    /* Skip overhang characters and high-value characters */
    if ((translated_char == ohc) || (translated_char >= 0370)) {
        goto return_width;
    }

    /* Handle zero-width characters */
    if (character_code & ZBIT) {
        goto return_width;
    }

    /* Get actual character width from font tables */
    width_result = get_character_width(translated_char);
    widthp = width_result;

return_width:
    return width_result;
}

/*
 * Calculate width for motion commands
 * 
 * Parameters:
 *   c - character with motion bits set
 * 
 * Returns:
 *   Motion width, or 0 for vertical motion
 */
static int calculate_motion_width(int c)
{
    int motion_value;

    /* Vertical motion - return 0 width */
    if (c & VMOT) {
        return 0;
    }

    /* Horizontal motion - extract and apply sign */
    motion_value = c & ~MOTV;
    if (c & NMOT) {
        motion_value = -motion_value;
    }

    return motion_value;
}

/*
 * Get character width from font tables
 * 
 * Parameters:
 *   c - character code
 * 
 * Returns:
 *   Character width in device units
 */
static int get_character_width(int c)
{
    int table_index;
    int char_width;

    /* Translate character through translation table */
    table_index = trtab[c] & BMASK;

    /* Skip control characters */
    if (table_index < 040) {
        return 0;
    }

    /* Look up width in font table */
    char_width = (*(t.codetab[table_index - 32]) & 0177) * t.Char;
    return char_width;
}

/*
 * Return the numeric code for the next special character request.
 * 
 * This function processes special character escape sequences and
 * returns the appropriate character code with formatting bits.
 * 
 * Returns:
 *   Character code with chbits formatting, or 0 if not found
 */
int setch(void)
{
    int request_code;
    int *table_ptr;
    int result_code;
    extern int chtab[];

    /* Get the special character request */
    if ((request_code = getrq()) == 0) {
        return 0;
    }

    /* Search character table for matching entry */
    for (table_ptr = chtab; *table_ptr != request_code; table_ptr += 2) {
        if (*table_ptr == 0) {
            return 0;  /* Not found */
        }
    }

    /* Return character code with formatting bits */
    result_code = *(table_ptr + 1) | chbits;
    return result_code;
}

/*
 * Search array 'font_array' for font number matching 'font_id'.
 * 
 * This function handles both numeric font positions (1-4) and
 * named font identifiers by searching the font label array.
 * 
 * Parameters:
 *   font_id    - font identifier (numeric or character)
 *   font_array - array of font labels to search
 * 
 * Returns:
 *   Font index (0-based), or -1 if not found
 */
int find(int font_id, int font_array[])
{
    int numeric_font;
    int search_index;

    /* Handle numeric font positions 1-4 */
    if (((numeric_font = font_id - '0') >= 1) && 
        (numeric_font <= 4) && 
        (numeric_font != smnt)) {
        return numeric_font - 1;  /* Convert to 0-based index */
    }

    /* Search font label array */
    for (search_index = 0; 
         font_array[search_index] != font_id; 
         search_index++) {
        if (font_array[search_index] == 0) {
            return -1;  /* Not found */
        }
    }

    return search_index;
}

/*
 * Update chbits to reflect the current point size and font.
 * 
 * This function encodes the current formatting state into chbits
 * and recalculates the space width for the current font.
 */
void mchbits(void)
{
    /* Encode point size and font into chbits */
    chbits = (((pts) << 2) | font) << (BYTE + 1);
    
    /* Calculate space width for current font */
    sps = width(' ' | chbits);
}

/*
 * Parse and apply a point-size change request.
 * 
 * This function handles point size specifications including:
 * - Relative changes (+n, -n)
 * - Absolute sizes (6-99 points)
 * - Single and double digit values
 */
void setps(void)
{
    int first_char;
    int second_digit;
    int point_size;

    /* Get first character of point size specification */
    first_char = getch() & CMASK;

    /* Handle relative point size changes */
    if ((first_char == '+') || (first_char == '-')) {
        second_digit = (ch = getch() & CMASK) - '0';
        if ((second_digit >= 0) && (second_digit <= 9)) {
            ch = 0;
            /* Adjust current point size by the relative amount */
            point_size = pts;
            if (first_char == '+') {
                point_size += second_digit;
            } else {
                point_size -= second_digit;
            }
            pts = point_size;
            mchbits();
            return;
        }
    }

    /* Convert to numeric value */
    point_size = first_char - '0';
    
    /* Validate single digit */
    if (point_size == 0) {
        return;  /* Invalid size */
    }

    /* Handle multi-digit point sizes */
    if ((point_size > 0) && (point_size <= 9)) {
        /* Check for second digit */
        if ((point_size <= 3)) {
            second_digit = (ch = getch() & CMASK) - '0';
            if ((second_digit >= 0) && (second_digit <= 9)) {
                point_size = 10 * point_size + second_digit;
                ch = 0;
            }
        }
        /* Apply the point size change */
        pts = point_size;
        mchbits();  /* Update character formatting bits */
    }
}

/*
 * Handle the \f request to change fonts.
 * 
 * This is the main entry point for font change commands.
 */
void caseft(void)
{
    skip();        /* Skip whitespace */
    setfont(1);    /* Set font with argument parsing */
}

/*
 * Switch to font position 'parse_arg'.
 * 
 * This function changes the current font, handling:
 * - Named fonts (R, I, B, S)
 * - Numeric font positions (1-4)
 * - Previous font recall (P)
 * - Special font handling (S)
 * 
 * Parameters:
 *   parse_arg - if non-zero, parse argument; otherwise use stored value
 */
void setfont(int parse_arg)
{
    int font_request;
    int new_font_index;

    /* Get font specification */
    if (parse_arg) {
        font_request = getrq();
    } else {
        font_request = getstr();
    }

    /* Handle special cases */
    if (!font_request || (font_request == 'P')) {
        /* Previous font or no argument */
        new_font_index = font1;
        goto apply_font;
    }

    if (font_request == 'S') {
        /* Special font - no change needed */
        return;
    }

    /* Look up font in font table */
    if ((new_font_index = find(font_request, fontlab)) == -1) {
        return;  /* Font not found */
    }

apply_font:
    /* Save current font and apply new one */
    font1 = font;
    font = new_font_index;
    mchbits();  /* Update character formatting bits */
}

/*
 * Compute width information for a string.
 * 
 * This function processes a delimited string and calculates:
 * - Total width
 * - Maximum height above baseline
 * - Maximum depth below baseline
 * - Character count
 */
void setwd(void)
{
    int current_char;
    int baseline;
    int total_width;
    int delimiter;
    int em_height;
    int vertical_offset;
    int saved_level;
    int saved_hp;
    int saved_font;
    int saved_font1;

    /* Initialize measurements */
    baseline = v.st = v.sb = total_width = v.ct = 0;

    /* Get delimiter character */
    if ((delimiter = getch() & CMASK) & MOT) {
        return;  /* Invalid delimiter */
    }

    /* Save current state */
    saved_hp = v.hp;
    saved_level = level;
    v.hp = level = 0;
    saved_font = font;
    saved_font1 = font1;
    setwdf++;

    /* Process characters until delimiter or newline */
    while ((((current_char = getch()) & CMASK) != delimiter) && !nlflg) {
        /* Add character width */
        total_width += width(current_char);

        if (!(current_char & MOT)) {
            /* Regular character - use standard em height */
            em_height = 2 * t.Halfline;
        } else if (current_char & VMOT) {
            /* Vertical motion */
            vertical_offset = current_char & ~MOTV;
            if (current_char & NMOT) {
                vertical_offset = -vertical_offset;
            }
            baseline = -vertical_offset;
            em_height = 0;
        } else {
            /* Horizontal motion - skip height calculation */
            continue;
        }

        /* Update maximum extents */
        if (baseline < v.sb) {
            v.sb = baseline;
        }
        if ((vertical_offset = baseline + em_height) > v.st) {
            v.st = vertical_offset;
        }
    }

    /* Finalize and restore state */
    nform = 0;
    setn1(total_width);
    v.hp = saved_hp;
    level = saved_level;
    font = saved_font;
    font1 = saved_font1;
    mchbits();
    setwdf = 0;
}

/*
 * Vertical motion command processor.
 * 
 * Sets up for vertical motion parsing with line spacing as default unit.
 * 
 * Returns:
 *   Encoded vertical motion value
 */
int vmot(void)
{
    dfact = lss;    /* Use line spacing as scaling factor */
    vflag++;        /* Set vertical motion flag */
    return mot();   /* Parse and return motion */
}

/*
 * Horizontal motion command processor.
 * 
 * Sets up for horizontal motion parsing with em as default unit.
 * 
 * Returns:
 *   Encoded horizontal motion value
 */
int hmot(void)
{
    dfact = EM;     /* Use em as scaling factor */
    return mot();   /* Parse and return motion */
}

/*
 * Parse a motion request and return an encoded motion value.
 * 
 * This function processes motion commands like \h'value' and \v'value',
 * parsing the numeric argument and encoding it with appropriate flags.
 * 
 * Returns:
 *   Encoded motion value with direction and type flags
 */
int mot(void)
{
    int motion_amount;
    int motion_direction;

    motion_direction = HOR;  /* Default to horizontal */
    getch();                 /* Consume delimiter */

    /* Parse motion amount */
    if ((motion_amount = tatoi()) != 0) {
        if (vflag) {
            motion_direction = VERT;
        }
        motion_amount = makem(quant(motion_amount, motion_direction));
    }

    getch();        /* Consume closing delimiter */
    vflag = 0;      /* Clear vertical flag */
    dfact = 1;      /* Reset scaling factor */
    
    return motion_amount;
}

/*
 * Return a half-line motion for underline or reverse operations.
 * 
 * This function generates vertical motions for:
 * - 'u': underline (half-line down)
 * - 'r': reverse (full-line up)
 * - other: half-line up
 * 
 * Parameters:
 *   operation - operation type ('u', 'r', or other)
 * 
 * Returns:
 *   Encoded vertical motion value
 */
int sethl(int operation)
{
    int motion_distance;

    motion_distance = t.Halfline;

    if (operation == 'u') {
        motion_distance = -motion_distance;      /* Underline: down */
    }
    
    return motion_distance;
}
