package cbib.db;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Date;
import java.util.List;
import java.util.Vector;

/**
 * 提醒服务
 * @author hankjin
 * todo
 */
public class Reminder {
	public int id;
	public String user;				//用户id
	public String notetype;			//提醒类型 短信/邮件 S/E
	public Date hwhen;				//提醒时间
	public String content;			//提醒内容

	/**
	*创建对象
	*默认全部为空
	*/
	public Reminder(){
	}
	/**
	 * 更新数据库
	 * @return
	 * @throws SQLException
	 */
	public boolean update2DB()throws SQLException{
		String sql = "update reminder set notetype=?, user=?, hwhen=?, content=? "
			+ " where id=?";
		DBConn conn = new DBConn();
		PreparedStatement stmt = conn.getStmt(sql);
		stmt.setString(1,this.notetype);		//提醒类型
		stmt.setString(2,this.user);			//用户
		stmt.setLong(3, this.hwhen.getTime());	//提醒时间
		stmt.setString(4, this.content);		//提醒内容
		stmt.setInt(5,this.id);					//
		boolean result = stmt.execute();
		conn.close();
		return result;
	}
	/**
	*根据ID获取对象
	*/
	public static Reminder getReminder(int id) throws SQLException{
		List<Reminder> result = getReminders("Select * from reminder where id="+id);
		return result.size() > 0? result.get(0) : null;
	}
	/**
	 * 根据ID删除提醒
	 * @param id
	 * @return
	 * @throws SQLException
	 */
	public static boolean delReminder(int id) throws SQLException{
		DBConn conn = new DBConn();
		Statement stmt = conn.getStmt();
		int result = stmt.executeUpdate("delete from reminder where id="+id);
		conn.close();
		return result > 0;		
	}
	/**
	*根据用户ID获取通知
	*/	
	public static List<Reminder> getUserReminders(String uid) throws SQLException{
		return getReminders("Select * from reminder where user='"+uid+"'");
	}	
	/**
	*根据SQL获取对象
	*/	
	public static List<Reminder> getReminders(String sql) throws SQLException{
		List<Reminder> reminders = new Vector<Reminder>();
		DBConn conn = new DBConn();
		ResultSet rs = conn.getRs(sql);
		while(rs.next()){
			Reminder reminder = new Reminder();
			reminder.id = rs.getInt("id");
			reminder.notetype = rs.getString("notetype");
			reminder.user = rs.getString("user");
			reminder.hwhen = new Date(rs.getLong("hwhen"));
			reminder.content = rs.getString("content");
			reminders.add(reminder);
		}
		conn.close();
		return reminders;
	}	
	/**
	*持久化到数据库
	*@return true:succeed; false: failed
	*/
	public static boolean persistent(Reminder reminder) throws SQLException{
		String sql = "insert into reminder(notetype, user, hwhen, content)"
			+ " values (?, ?, ?, ?)";
		DBConn conn = new DBConn();
		PreparedStatement stmt = conn.getStmt(sql);
		stmt.setString(1,reminder.notetype);		//提醒类型
		stmt.setString(2,reminder.user);			//用户
		stmt.setLong(3, reminder.hwhen.getTime());	//提醒时间
		stmt.setString(4, reminder.content);		//提醒内容
		boolean result = stmt.execute();
		conn.close();
		return result;
	}
	/**
	 * 测试
	 * @param args
	 */
	public static void main(String args[])throws Exception{
		Reminder reminder;
		//*/
		reminder = new Reminder();
		reminder.user = "hankjin";
		reminder.hwhen = new Date();
		reminder.notetype = "S";
		reminder.content = "Hello, It is time";
		Reminder.persistent(reminder);
		//*/
		reminder = Reminder.getReminder(1);
		System.out.println(reminder.user);
	}
}
