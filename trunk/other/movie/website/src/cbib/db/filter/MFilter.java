package cbib.db.filter;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import cbib.db.MovieInfo;

public class MFilter {
	public static final String ANY = "全部";
	public Map<String, String> conditions;
	public List<String> raws;
	public MFilter(){
		conditions = new HashMap<String, String>();
		raws = new ArrayList<String>();
	}
	public void addCondition(String column, String value){
		conditions.put(column, value);
	}
	public void addRawCondition(String condition){
		raws.add(" "+condition+" ");
	}
	public String toString(){
		String result = "";
		for(String column:conditions.keySet()){
			result += " and "+column + " like '%" + conditions.get(column)+"%' "; 
		}
		for(String con: raws){
			result += " and " + con;
		}
		System.out.println(result);
		return result;
	}
	public static void main(String args[])throws Exception{
		MFilter filter = new MFilter();
		String zone = "汉口";
		String category = "全部";
		String movie = "全部";
		String director = "全部";
		String actor = "葛优";
		String hdate = "全部";
		String cinema = "全部";
		if(zone != null && !zone.equals("全部"))
			filter.addCondition("cinema.zone",zone);
		if(category != null && !category.equals("全部"))
			filter.addCondition("movie.category",category);
		if(movie != null && !movie.equals("全部"))
			filter.addCondition("movie.name",movie);
		if(director != null && !director.equals("全部"))
			filter.addCondition("movie.director", director);
		if(actor != null && !actor.equals("全部"))
			filter.addCondition("movie.actor", actor);
		if(hdate != null && !hdate.equals("全部"))
			filter.addCondition("movieinfo.hdate", hdate);
		if(cinema != null && !cinema.equals("全部"))
			filter.addCondition("cinema.name", cinema);
		List<MovieInfo> infos = MovieInfo.getMovieInfos(filter);
		for(MovieInfo info: infos){
			System.out.println(info);
		}
		System.out.println("Total:"+infos.size());
	}
}
