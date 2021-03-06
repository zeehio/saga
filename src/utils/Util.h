/* Saga - Un transcriptor fon�tico para el idioma espa�ol
 *
 * Copyright (C) 1993-2009  Albino Nogueiras Rodr�guez y Jos� B. Mari�o
 *       TALP - Universitat Polit�cnica de Catalunya, ESPA�A
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef	UTIL_H
#define	UTIL_H

char **MatStr(const char *);
char **MatStrChr(const char *, const char *);
size_t MatStrLength(char **mat);
void LiberaMatStr(char **MatChr);

int safe_strcat(char **dest, const char *src, size_t * dest_size,
                size_t * dest_strlen);
size_t utf8_to_latin9(char *const output, const char *const input,
                      const size_t length);
char* Saga_concat(const char *s1, const char *s2);

#endif /* defined (UTIL_H)                                     */
