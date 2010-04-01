package cbib.search;

import java.util.Date;
import java.util.HashSet;
import java.util.Set;

/**
 * 搜索API
 * @author hankjin
 *
 */
public class SearchAPI {
	/**
	 * 根据关键词进行简单搜索
	 * @param keywords 关键词
	 * @return 搜索结果
	 */
	public static Set<SearchResult> search(String []keywords){
		Set<SearchResult> sr = new HashSet<SearchResult>();
		return sr;
	}
	/**
	 * 根据时间间隔搜索
	 * @param from
	 * @param begin
	 * @return
	 */
	public static Set<SearchResult> search(Date from, Date begin){
		Set<SearchResult> sr = new HashSet<SearchResult>();
		return sr;
	}
}
