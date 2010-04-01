package cbib.db;

import java.sql.SQLException;
import java.util.List;
import java.util.Vector;

/**
 * 通知服务
 * @author hankjin
 * todo
 */
public class Notify {
	
	/**
	*创建对象
	*默认全部为空
	*/
	public Notify(){
	}
	
	/**
	*根据ID获取对象
	*/
	public static Notify getNotify(int id) throws SQLException{
		List<Notify> result = getNotifys("Select * from notify where id="+id);
		return result.size() > 0? result.get(0) : null;
	}
	/**
	*根据用户ID获取通知
	*/	
	public static List<Notify> getUserNotifys(String uid) throws SQLException{
		return getNotifys("Select * from notify where user='"+uid+"'");
	}	
	/**
	*根据SQL获取对象
	*/	
	public static List<Notify> getNotifys(String sql) throws SQLException{
		List<Notify> notifys = new Vector<Notify>();
		return notifys;
	}	
	/**
	*持久化到数据库
	*@return true:succeed; false: failed
	*/
	public static boolean persistent(Notify movie) throws SQLException{
		return true;
	}
}
