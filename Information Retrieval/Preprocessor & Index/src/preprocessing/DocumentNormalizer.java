package preprocessing;

import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import containers.Document;
import tools.Loader;

public class DocumentNormalizer {
	private String[] prefixes = new String[123];
	
	public DocumentNormalizer() {
		prefixes = new Loader().populateArray(prefixes, "files/prefixes.txt");
	}
	
	public void normalize(Document preppedDocument) {
		acronyms(preppedDocument);
		alphabetDigit(preppedDocument);
		digitAlphabet(preppedDocument);
		prefixedWords(preppedDocument);
		hyphenatedWords(preppedDocument);
		slashedWords(preppedDocument);
		numbers(preppedDocument);
		dates(preppedDocument);
		specialCharacters(preppedDocument);
	}
	

	
	/*
	 *  DESIGN ISSUE: u.s. --> us will not be saved during parsing due to "us" being a stop word, yet 
	 *  in the context of an acronym u.s. is meaningful as a token. How do we preserve meaning of
	 *  acronyms that masquerade as normal words?
	 */
	
	public void acronyms(Document document) {
		Pattern acronymPattern = Pattern.compile("([a-z])(\\.[a-z])+(\\.)?");
		Matcher acronymMatcher = acronymPattern.matcher(document.text());
		
		while (acronymMatcher.find()) {
			String escapedAcronym = acronymMatcher.group(0).replace(".", "\\.");
			String normalizedAcronym = acronymMatcher.group(0).replace(".", "");
			document.update(document.text().replaceAll(escapedAcronym, normalizedAcronym));
		}
	}
	
	public void alphabetDigit(Document document) {
		Pattern alphaDigitPattern = Pattern.compile("([a-z]+)(-)([0-9]+)");
		Matcher alphaDigitMatcher = alphaDigitPattern.matcher(document.text());
		
		while (alphaDigitMatcher.find()) {
			String alpha = alphaDigitMatcher.group(1);
			String digit = alphaDigitMatcher.group(3);
			
			if (alpha.length() >= 3) {
				document.update(document.text().replaceAll(alpha + "-" + digit, alpha + digit + " " + alpha));
			} else {
				document.update(document.text().replaceAll(alpha + "-" + digit, digit));
			}
		}
	}
	
	public void digitAlphabet(Document document) {
		Pattern digitAlphaPattern = Pattern.compile("([0-9]+)(-)([a-z]+)");
		Matcher digitAlphaMatcher = digitAlphaPattern.matcher(document.text());
		
		while (digitAlphaMatcher.find()) {
			String digit = digitAlphaMatcher.group(1);
			String alpha = digitAlphaMatcher.group(3);
			
			if (alpha.length() >= 3) {
				document.update(document.text().replaceAll(digit + "-" + alpha, digit + alpha + " " + alpha));
			} else {
				document.update(document.text().replaceAll(digit + "-" + alpha, digit));
			}
		}
	}

	
	public void prefixedWords(Document document) {
		for (String prefix : prefixes) {
			Pattern prefixPattern = Pattern.compile("(" + prefix + ")(-)([a-z]+)");
			Matcher prefixMatcher = prefixPattern.matcher(document.text());
			
			while (prefixMatcher.find()) {
				String word = prefixMatcher.group(3);
				document.update(document.text().replaceAll(prefix + "-" + word, prefix + word + " " + word));
			}
		}
	}
	
	public void hyphenatedWords(Document document) {
		Pattern triHyphenatedPattern = Pattern.compile("([a-z]+)(-)([a-z]+)(-)([a-z]+)");
		Matcher triHyphenatedMatcher = triHyphenatedPattern.matcher(document.text());
		
		while (triHyphenatedMatcher.find()) {
			String word1 = triHyphenatedMatcher.group(1);
			String word2 = triHyphenatedMatcher.group(3);
			String word3 = triHyphenatedMatcher.group(5);
			
			document.update(document.text().replaceAll(triHyphenatedMatcher.group(0), 
					word1 + word2 + word3 + " " + word1 + " "  + word2 + " " + word3));
		}
		
		Pattern biHyphenatedPattern = Pattern.compile("([a-z]+)(-)([a-z]+)");
		Matcher biHyphenatedMatcher = biHyphenatedPattern.matcher(document.text());
		
		while (biHyphenatedMatcher.find()) {
			String word1 = biHyphenatedMatcher.group(1);
			String word2 = biHyphenatedMatcher.group(3);
			
			document.update(document.text().replaceAll(biHyphenatedMatcher.group(0), 
					word1 + word2 + " " + word1 + " "  + word2));
		}	
	}
	
	public void slashedWords(Document document) {
		Pattern triSlashedPattern = Pattern.compile("([a-z]+)(/)([a-z]+)(/)([a-z]+)");
		Matcher triSlashedMatcher = triSlashedPattern.matcher(document.text());
		
		while (triSlashedMatcher.find()) {
			String word1 = triSlashedMatcher.group(1);
			String word2 = triSlashedMatcher.group(3);
			String word3 = triSlashedMatcher.group(5);
			
			document.update(document.text().replaceAll(triSlashedMatcher.group(0), 
					word1 + word2 + word3 + " " + word1 + " "  + word2 + " " + word3));
		}
		
		Pattern biSlashedPattern = Pattern.compile("([a-z]+)(/)([a-z]+)");
		Matcher biSlashedMatcher = biSlashedPattern.matcher(document.text());
		
		while (biSlashedMatcher.find()) {
			String word1 = biSlashedMatcher.group(1);
			String word2 = biSlashedMatcher.group(3);
			
			document.update(document.text().replaceAll(biSlashedMatcher.group(0), 
					word1 + word2 + " " + word1 + " "  + word2));
		}	
	}
	
	public void numbers(Document document) {
		Pattern numbersPattern = Pattern.compile("([0-9]{1,3},?)+(\\.[0-9]{1,})?");
		Matcher numbersMatcher = numbersPattern.matcher(document.text());
		
		while (numbersMatcher.find()) {
			String value = numbersMatcher.group(0);
			
			if (numbersMatcher.group(2) != null) {
				value = value.replace(numbersMatcher.group(2), "");
			}
			
			value = value.replaceAll(",", "");
			
			document.update(document.text().replaceAll(numbersMatcher.group(0), value));
		}
	}
	
	/*
	 * DESIGN: store dates as a single special token? Or parse normally later?
	 */
	
	public void dates(Document document) {
		removeInvalidShortDates(document);
		normalizeShortDates(document);
		filterLongDates(document);
	}
	
	public void removeInvalidShortDates(Document document) {
		Pattern pattern = Pattern.compile("([0-9]+)?(\\/|-)([0-9]+)?(\\/|-)([0-9]+)?");
		Matcher matcher = pattern.matcher(document.text());
		
		while (matcher.find()) {
			if (matcher.group(1) == null || 
				matcher.group(3) == null || 
				matcher.group(5) == null || 
				Integer.parseInt(matcher.group(1)) == 0 ||
				Integer.parseInt(matcher.group(3)) == 0 ||
				Integer.parseInt(matcher.group(5)) == 0 ||
				
				(Integer.parseInt(matcher.group(1)) == 1 && Integer.parseInt(matcher.group(3)) > 31) ||
				// Corner case: leap year
				(Integer.parseInt(matcher.group(1)) == 2 && Integer.parseInt(matcher.group(3)) > 29) ||
				(Integer.parseInt(matcher.group(1)) == 3 && Integer.parseInt(matcher.group(3)) > 31) ||
				(Integer.parseInt(matcher.group(1)) == 4 && Integer.parseInt(matcher.group(3)) > 30) ||
				(Integer.parseInt(matcher.group(1)) == 5 && Integer.parseInt(matcher.group(3)) > 31) ||
				(Integer.parseInt(matcher.group(1)) == 6 && Integer.parseInt(matcher.group(3)) > 30) ||
				(Integer.parseInt(matcher.group(1)) == 7 && Integer.parseInt(matcher.group(3)) > 31) ||
				(Integer.parseInt(matcher.group(1)) == 8 && Integer.parseInt(matcher.group(3)) > 31) ||
				(Integer.parseInt(matcher.group(1)) == 9 && Integer.parseInt(matcher.group(3)) > 30) ||
				(Integer.parseInt(matcher.group(1)) == 10 && Integer.parseInt(matcher.group(3)) > 31) ||
				(Integer.parseInt(matcher.group(1)) == 11 && Integer.parseInt(matcher.group(3)) > 30) ||
				(Integer.parseInt(matcher.group(1)) == 12 && Integer.parseInt(matcher.group(3)) > 31) ||
					
				Integer.parseInt(matcher.group(1)) > 12 ||
				Integer.parseInt(matcher.group(3)) > 31) {
				
				document.update(document.text().replaceAll(matcher.group(0), ""));
			}
		}
	}
	
	public void normalizeShortDates(Document document) {
		HashMap<String, String> months = new Loader().buildDictionary("files/months.txt", 2);
		
		String shortDateRegex = "(0[1-9]|[0-9]{1,2})(\\/|-)(0[1-9]|[0-9]{1,2})(\\/|-)([0-9]{2})?([0-9]{2})";
		
		Pattern shortDatePattern = Pattern.compile(shortDateRegex);
		Matcher shortDateMatcher = shortDatePattern.matcher(document.text());
		
		while (shortDateMatcher.find()) {
			String date = shortDateMatcher.group(0);
			
			date = date.replaceFirst(shortDateMatcher.group(1), months.get(shortDateMatcher.group(1)));
			date = date.replace(shortDateMatcher.group(2), " ");
			
			if (shortDateMatcher.group(5) == null) {
				if (Integer.parseInt(shortDateMatcher.group(6)) <= 16) {
					date = date.replaceFirst(shortDateMatcher.group(6), "20" + shortDateMatcher.group(6));
				} else {
					date = date.replaceFirst(shortDateMatcher.group(6), "19" + shortDateMatcher.group(6));
				}
			}	
			document.update(document.text().replaceFirst(shortDateMatcher.group(0), date));
		}
	}
	
	public void filterLongDates(Document document) {
		String regex = "(january|february|march|april|may|june|july|august|september|october|november|december)"
				+ "( )([0-9]+)(t?h?,? )([0-9]+)";
		
		Pattern pattern = Pattern.compile(regex);
		Matcher matcher = pattern.matcher(document.text());
		
		while (matcher.find()) {
			String month = matcher.group(1);
			String day = matcher.group(3);
			String year = matcher.group(5);
			
			if (month == null || 
				day == null || 
				year == null || 
				Integer.parseInt(day) == 0 ||
				Integer.parseInt(year) == 0 ||
				
				(month == "janurary" && Integer.parseInt(day) > 31) ||
				// Corner case: leap year
				(month == "february" && Integer.parseInt(day) > 29) ||
				(month == "march" && Integer.parseInt(day) > 31) ||
				(month == "april" && Integer.parseInt(day) > 30) ||
				(month == "may" && Integer.parseInt(day) > 31) ||
				(month == "june" && Integer.parseInt(day) > 30) ||
				(month == "july" && Integer.parseInt(day) > 31) ||
				(month == "august" && Integer.parseInt(day) > 31) ||
				(month == "september" && Integer.parseInt(day) > 30) ||
				(month == "october" && Integer.parseInt(day) > 31) ||
				(month == "november" && Integer.parseInt(day) > 30) ||
				(month == "december" && Integer.parseInt(day) > 31) ||
					
				Integer.parseInt(day) > 31) {
				
				document.update(document.text().replaceAll(matcher.group(0), ""));
			} else {
				
				// Save three different formats for partial matching
				
				if (document.specialTokens().get(month + "_" + day + "_" + year) == null) {
					document.saveToken(month + "_" + day + "_" + year);
				} else {
					document.incrementTokenFreq(month + "_" + day + "_" + year);
				}
				
				if (document.specialTokens().get(month + "_" + day) == null) {
					document.saveToken(month + "_" + day);
				} else {
					document.incrementTokenFreq(month + "_" + day);
				}
				
				if (document.specialTokens().get(month + "_" + year) == null) {
					document.saveToken(month + "_" + year);
				} else {
					document.incrementTokenFreq(month + "_" + year);
				}
				
				document.update(document.text().replaceFirst(matcher.group(0), ""));
			}
		}
	}
	
	public void specialCharacters(Document document) {
		document.update(document.text().replaceAll("[^a-zA-Z0-9\\s]", ""));
//		document.update(document.text().replace("`", ""));
//		document.update(document.text().replace("~", ""));
//		document.update(document.text().replace("!", ""));
//		document.update(document.text().replace("@", ""));
//		document.update(document.text().replace("#", ""));
//		document.update(document.text().replace("$", ""));
//		document.update(document.text().replace("%", ""));
//		document.update(document.text().replace("^", ""));
//		document.update(document.text().replace("&", ""));
//		document.update(document.text().replace("*", ""));
//		document.update(document.text().replace("(", ""));
//		document.update(document.text().replace(")", ""));
//		document.update(document.text().replace("-", ""));
//		document.update(document.text().replace("_", ""));
//		document.update(document.text().replace("=", ""));
//		document.update(document.text().replace("+", ""));
//		document.update(document.text().replace("[", ""));
//		document.update(document.text().replace("{", ""));
//		document.update(document.text().replace("]", ""));
//		document.update(document.text().replace("}", ""));
//		document.update(document.text().replace("\\", ""));
//		document.update(document.text().replace("|", ""));
//		document.update(document.text().replace(";", ""));
//		document.update(document.text().replace(":", ""));
//		document.update(document.text().replace("'", ""));
//		document.update(document.text().replace("\"", ""));
//		document.update(document.text().replace(",", ""));
//		document.update(document.text().replace("<", ""));
//		document.update(document.text().replace(".", ""));
//		document.update(document.text().replace(">", ""));
//		document.update(document.text().replace("/", ""));
//		document.update(document.text().replace("?", ""));
	}
}
