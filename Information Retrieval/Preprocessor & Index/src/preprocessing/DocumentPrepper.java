package preprocessing;

import java.util.Hashtable;
import java.util.LinkedList;
import java.util.regex.*;
import containers.Document;
import org.apache.commons.lang3.StringEscapeUtils;

public class DocumentPrepper {
	public DocumentPrepper() {
		
	}
	
	public Document prepare(String rawDoc) {
		Document preppedDocument = cleanAndKey(rawDoc);
		caseFolding(preppedDocument);
		extractSpecialTokens(preppedDocument);
		
		return preppedDocument;
	}
	
	public Document cleanAndKey(String rawDoc) {
		String DOCNO = null;
		String TEXT = null;
					
		Pattern DOCNO_pattern = Pattern.compile("(<DOCNO> )(.*)( </DOCNO>)");
		Matcher DOCNO_matcher = DOCNO_pattern.matcher(rawDoc);
		
		if (DOCNO_matcher.find()) {
			DOCNO = DOCNO_matcher.group(2);
			TEXT = DOCNO_matcher.replaceFirst("");
		}
		
		Pattern commentPattern = Pattern.compile("<!--.*-->");
		Matcher commentMatcher = commentPattern.matcher(TEXT);
		TEXT = commentMatcher.replaceAll("");
		
		Pattern parentPattern = Pattern.compile("(<PARENT>)(.*)(</PARENT>)");
		Matcher parentMatcher = parentPattern.matcher(TEXT);
		TEXT = parentMatcher.replaceFirst("");
		
		Pattern closeTagPattern = Pattern.compile("</.*>");
		Matcher closeTagMatcher = closeTagPattern.matcher(TEXT);
		TEXT = closeTagMatcher.replaceAll("");
		
		Pattern openTagPattern = Pattern.compile("<.*>");
		Matcher openTagMatcher = openTagPattern.matcher(TEXT);
		TEXT = openTagMatcher.replaceAll("");
		
		TEXT = TEXT.replaceAll("&hyph;", "-");
		TEXT = TEXT.replaceAll("&blank;", " ");
		TEXT = TEXT.replaceAll("&cir;", "");

		Pattern HTML4_Pattern = Pattern.compile("&[a-z]+;");
		Matcher HTML4_Matcher = HTML4_Pattern.matcher(TEXT);
		
		while (HTML4_Matcher.find()) {
			TEXT = HTML4_Matcher.replaceAll(" " + HTML4_Matcher.group(0) + " ");
		}
		
		TEXT = StringEscapeUtils.unescapeHtml4(TEXT);
		TEXT = TEXT.replaceAll("§", "");
		
		return new Document(DOCNO, TEXT);
	}
	
	// DO CASE FOLDING FIRST FOR THE LOVE OF GOD
	
	public void caseFolding(Document document) {
		document.update(document.text().toLowerCase());
	}
	
	// includes everything except dates, which are better saved for post-normalization
	
	public void extractSpecialTokens(Document document) {
		extractURLs(document);
		extractEmails(document);
		extractMonetaries(document);
		extractIPs(document);
		extractFilenames(document);
		
//		for (String docKey : documents.keySet()) {
//			for (String tokenKey : documents.get(docKey).specialTokens().keySet()) {
//				System.out.println("Special token \'" + tokenKey + "\' frequency: " + 
//			documents.get(docKey).specialTokens().get(tokenKey).tf());
//			}
//		}
	}
	
	public void extractFilenames(Document document) {
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
		Matcher fileMatcher = filePattern.matcher(document.text());
		
		while (fileMatcher.find()) {
			if (fileMatcher.group(1).length() > 1 && fileMatcher.group(3).length() > 1) {
				
				if (document.specialTokens().get(fileMatcher.group(0)) == null) {
					document.saveToken(fileMatcher.group(0));
				} else {
					document.incrementTokenFreq(fileMatcher.group(0));
				}
				document.update(document.text().replaceFirst(fileMatcher.group(0), ""));
			}
		}
	}
	
	public void extractMonetaries(Document document) {
		Pattern monetaryPattern = Pattern.compile("\\$([0-9]{1,3},?)+(\\.[0-9]{2})?");
		Matcher monetaryMatcher = monetaryPattern.matcher(document.text());
		
		while (monetaryMatcher.find()) {
			String value = monetaryMatcher.group(0);
			
			if (monetaryMatcher.group(2) != null) {
				value = value.replace(monetaryMatcher.group(2), "");
			}
			
			value = value.replaceAll(",", "");
			
			if (document.specialTokens().get(value) == null) {
				document.saveToken(value);
			} else {
				document.incrementTokenFreq(value);
			}
			document.update(document.text().replaceFirst("\\" + monetaryMatcher.group(0), ""));
		}
	}
	
	/*
	 * Follow this article for email forms:
	 *  http://stackoverflow.com/questions/2049502/what-characters-are-allowed-in-email-address
	 */
	
	public void extractEmails(Document document) {
		Pattern emailPattern = Pattern.compile(".+@.+\\.[a-z]+");
		Matcher emailMatcher = emailPattern.matcher(document.text());
		
		while (emailMatcher.find()) {
			if (document.specialTokens().get(emailMatcher.group(0)) == null) {
				document.saveToken(emailMatcher.group(0));					
			} else {
				document.incrementTokenFreq(emailMatcher.group(0));
			}

			document.update(document.text().replaceFirst(emailMatcher.group(0), ""));
		}
	}
	
	public void extractIPs(Document document) {
		Pattern IP_Pattern = Pattern.compile("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");
		Matcher IP_Matcher = IP_Pattern.matcher(document.text());
		
		while (IP_Matcher.find()) {
			if (document.specialTokens().get(IP_Matcher.group(0)) == null) {
				document.saveToken(IP_Matcher.group(0));
			} else {
				document.incrementTokenFreq(IP_Matcher.group(0));
			}
			document.update(document.text().replaceFirst(IP_Matcher.group(0), ""));
		}
	}
	
	public void extractURLs(Document document) {
	String URLregex = "(http:\\/\\/|https:\\/\\/)?(www\\.)?([a-zA-Z0-9]+\\.)?[a-zA-Z0-9]+\\.[a-z]{2,4}";
				
		Pattern URL_Pattern = Pattern.compile(URLregex);
		Matcher URL_Matcher = URL_Pattern.matcher(document.text());
		
		while (URL_Matcher.find()) {
			if (document.specialTokens().get(URL_Matcher.group(0)) == null) {
				document.saveToken(URL_Matcher.group(0));					
			} else {
				document.incrementTokenFreq(URL_Matcher.group(0));
			}
			document.update(document.text().replaceFirst(URL_Matcher.group(0), ""));
		}	
	}
}
