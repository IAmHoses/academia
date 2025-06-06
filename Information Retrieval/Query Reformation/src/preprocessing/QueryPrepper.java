package preprocessing;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import containers.Query;

public class QueryPrepper {
	public QueryPrepper() {
		
	}
	
	public Query prepare(Query query) {
		removeTaggedLines(query);
		caseFold(query);
		extractSpecialTokens(query);
		
		return query;
	}
	
	public void removeTaggedLines(Query query) { 
		Pattern pattern = Pattern.compile("<[a-z]+> [a-zA-Z]+: ?");
		Matcher matcher = pattern.matcher(query.string());
			
		if (matcher.find()) {
			query.updateString(matcher.replaceAll(""));
		}
	}
	
	public void caseFold(Query query) {
		query.updateString(query.string().toLowerCase());
	}
	
	// includes everything except dates, which are better saved for post-normalization
	
	public void extractSpecialTokens(Query query) {
		extractEmails(query);
		extractURLs(query);
		extractFilenames(query);
		extractMonetaries(query);
		extractIPs(query);
	}
	
	public void extractURLs(Query query) {
	String URLregex = "(http:\\/\\/|https:\\/\\/)?(www\\.)?([a-zA-Z0-9]+\\.)?[a-zA-Z0-9]+\\.[a-z]{2,4}";
				
		Pattern URL_Pattern = Pattern.compile(URLregex);
		Matcher URL_Matcher = URL_Pattern.matcher(query.string());
		
		while (URL_Matcher.find()) {
			if (query.tokens().get(URL_Matcher.group(0)) == null) {
				query.saveToken(URL_Matcher.group(0));					
			} else {
				query.incrementTokenFreq(URL_Matcher.group(0));
			}
			query.updateString(query.string().replaceFirst(URL_Matcher.group(0), ""));
		}	
	}
	
	/*
	 * Follow this article for email forms:
	 *  http://stackoverflow.com/questions/2049502/what-characters-are-allowed-in-email-address
	 */
	
	public void extractEmails(Query query) {
		Pattern emailPattern = Pattern.compile(".+@.+\\.[a-z]+");
		Matcher emailMatcher = emailPattern.matcher(query.string());
		
		while (emailMatcher.find()) {
			if (query.tokens().get(emailMatcher.group(0)) == null) {
				query.saveToken(emailMatcher.group(0));					
			} else {
				query.incrementTokenFreq(emailMatcher.group(0));
			}

			query.updateString(query.string().replaceFirst(emailMatcher.group(0), ""));
		}
	}
	
	public void extractFilenames(Query query) {
		/*
		 * REGEX: ([a-zA-Z]+)(\\.)([a-zA-Z]+)
		 * 
		 * DESIGN: make sure you test that groups 1 and 3 have a length greater than one.
		 * If a file is named something like a.p, then it will simply be mistaken as an
		 * acronym. While you lose some information, that's the fault of the fool who
		 * named a file such a stupid name.
		 */
			
		/*
		 * BUG: more complex filename regex is not matching due to improper closing of groups () ?
		 */
			
//		Pattern filePattern = Pattern.compile("([a-zA-Z0-9`~!@#$^&()-_=+\\[\\]{},;']+)(\\.)([a-z]+)");
		Pattern filePattern = Pattern.compile("([a-z0-9]{2,})(\\.)([a-z]{2,})");
		Matcher fileMatcher = filePattern.matcher(query.string());
		
		while (fileMatcher.find()) {
			if (fileMatcher.group(1).length() > 1 && fileMatcher.group(3).length() > 1) {
				
				if (query.tokens().get(fileMatcher.group(0)) == null) {
					query.saveToken(fileMatcher.group(0));
				} else {
					query.incrementTokenFreq(fileMatcher.group(0));
				}
				query.updateString(query.string().replaceFirst(fileMatcher.group(0), ""));
			}
		}
	}
	
	public void extractMonetaries(Query query) {
		Pattern monetaryPattern = Pattern.compile("\\$([0-9]{1,3},?)+(\\.[0-9]{2})?");
		Matcher monetaryMatcher = monetaryPattern.matcher(query.string());
		
		while (monetaryMatcher.find()) {
			String value = monetaryMatcher.group(0);
			
			if (monetaryMatcher.group(2) != null) {
				value = value.replace(monetaryMatcher.group(2), "");
			}
			
			value = value.replaceAll(",", "");
			
			if (query.tokens().get(value) == null) {
				query.saveToken(value);
			} else {
				query.incrementTokenFreq(value);
			}
			query.updateString(query.string().replaceFirst("\\" + monetaryMatcher.group(0), ""));
		}
	}
	
	public void extractIPs(Query query) {
		Pattern IP_Pattern = Pattern.compile("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");
		Matcher IP_Matcher = IP_Pattern.matcher(query.string());
		
		while (IP_Matcher.find()) {
			if (query.tokens().get(IP_Matcher.group(0)) == null) {
				query.saveToken(IP_Matcher.group(0));
			} else {
				query.incrementTokenFreq(IP_Matcher.group(0));
			}
			query.updateString(query.string().replaceFirst(IP_Matcher.group(0), ""));
		}
	}
}
