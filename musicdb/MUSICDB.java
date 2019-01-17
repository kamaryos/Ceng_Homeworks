package ceng.ceng351.musicdb;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;


public class MUSICDB implements IMUSICDB {


      private static String user = "e2099422";
      private static String password = "8d8bbab5";
      private static String host = "144.122.71.57";
      private static String database = "db2099422";
      private static int port = 8084;

      private Connection con = null;

      public MUSICDB(){}

      public void initialize(){

        String url = "jdbc:mysql://" + this.host + ":" + this.port + "/" + this.database;

        try {
            Class.forName("com.mysql.jdbc.Driver");
            this.con =  DriverManager.getConnection(url, this.user, this.password);
        } catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        }
      }

      public int createTables(){

        List<String> queryCreateTables = new ArrayList<String>();
        int result;
        int numberofTablesInserted = 0;

        String queryCreateUserTable = "create table user (" +
                                                                "userID int not null,"+
                                                                "userName varchar(60),"+
                                                                "email varchar(30),"+
                                                                "password varchar(30),"+
                                                                "primary key(userID))";
        String queryCreateArtistTable = "create table artist (" +
                                                                "artistID int not null,"+
                                                                "artistName varchar(60),"+
                                                                "primary key(artistID))";
        String queryCreateAlbumTable = "create table album (" +
                                                                "albumID int not null,"+
                                                                "title varchar(60),"+
                                                                "albumGenre varchar(30),"+
                                                                "albumRating double,"+
                                                                "releaseDate date,"+
                                                                "artistID int ,"+
                                                                "primary key(albumID),"+
                                                                "foreign key(artistID) references artist(artistID))";

        String queryCreateSongTable = "create table song (" +
                                                               "songID int not null,"+
                                                               "songName varchar(60),"+
                                                               "genre varchar(30),"+
                                                               "rating double,"+
                                                               "artistID int,"+
                                                               "albumID int,"+
                                                               "primary key(songID),"+
                                                               "foreign key(artistID) references artist(artistID),"+
                                                               "foreign key(albumID) references album(albumID))";

        String queryCreateListenTable = "create table listen (" +
                                                               "userID int not null,"+
                                                               "songID int not null,"+
                                                               "genre varchar(30),"+
                                                               "listenCount double,"+
                                                               "primary key(userID,songID),"+
                                                               "foreign key(userID) references user(userID),"+
                                                               "foreign key(songID) references song(songID) on delete cascade)";
       queryCreateTables.add(0,queryCreateUserTable);
       queryCreateTables.add(1,queryCreateArtistTable);
       queryCreateTables.add(2,queryCreateAlbumTable);
       queryCreateTables.add(3,queryCreateSongTable);
       queryCreateTables.add(4,queryCreateListenTable);

       for(int i = 0 ; i < 5 ; i++) {
          try{
             Statement statement = con.createStatement();

             
             result = statement.executeUpdate(queryCreateTables.get(i));
             
             numberofTablesInserted++;

             //close
             statement.close();
         }
         catch (SQLException e) {
             e.printStackTrace();
         }
       }

       return numberofTablesInserted;

      }

      public int dropTables(){

        int result;
        int numberofTablesDropped = 0;

        List<String> queryDropTables = new ArrayList<String>();

        String queryDropUserTable = "drop table if exists user";
        String queryDropArtistTable = "drop table if exists artist";
        String queryDropAlbumTable = "drop table if exists album";
        String queryDropSongTable = "drop table if exists song";
        String queryDropListenTable = "drop table if exists listen";


        queryDropTables.add(0,queryDropListenTable);
        queryDropTables.add(1,queryDropSongTable);
        queryDropTables.add(2,queryDropAlbumTable);
        queryDropTables.add(3,queryDropArtistTable);
        queryDropTables.add(4,queryDropUserTable);

        for(int i = 0 ; i < 5 ; i++) {
           try{
              Statement statement = con.createStatement();

              result = statement.executeUpdate(queryDropTables.get(i));
              numberofTablesDropped++;
              

              //close
              statement.close();
          }

          catch (SQLException e) {
              e.printStackTrace();
          }

        }
        return numberofTablesDropped;

      }

      public int insertAlbum(Album[] albums) {
        int result = 0;
        int numberofAlbumsInserted = 0;

        List<String> insertAlbums = new ArrayList<String>();

        
        for(int i = 0 ; i < albums.length ; i++){
          String query = "insert into album values (" +
                          albums[i].getAlbumID()+ ",\"" +
                          albums[i].getTitle() + "\",\"" +
                          albums[i].getAlbumGenre() + "\"," +
                          albums[i].getAlbumRating() + ",\"" +
                          albums[i].getReleaseDate() + "\"," +
                          albums[i].getArtistID() + ")";

         insertAlbums.add(i,query);
        }


        for(int i = 0 ; i < insertAlbums.size() ; i++){

          try {
              Statement st = con.createStatement();
              result = st.executeUpdate(insertAlbums.get(i));
              numberofAlbumsInserted++;
              st.close();

          } catch (SQLException e) {
              //e.printStackTrace();
              if (e.toString().contains("SQLIntegrityConstraintViolationException")){
                  //throw new AlreadyInsertedException();
              }
            }
        }
        return numberofAlbumsInserted;
      }

      public int insertArtist(Artist[] artists){
        int result = 0;
        int numberofArtistsInserted = 0;

        List<String> insertArtists = new ArrayList<String>();


        for(int i = 0 ; i < artists.length ; i++){
          String query = "insert into artist values (" +
                          artists[i].getArtistID()+ ",\"" +
                          artists[i].getArtistName() + "\" )";
         insertArtists.add(i,query);
        }


        for(int i = 0 ; i < insertArtists.size() ; i++){

          try {
              Statement st = con.createStatement();
              result = st.executeUpdate(insertArtists.get(i));
              numberofArtistsInserted++;

              //Close
              st.close();

          } catch (SQLException e) {
              //e.printStackTrace();
              if (e.toString().contains("SQLIntegrityConstraintViolationException")){
                  //throw new AlreadyInsertedException();
              }
            }
        }
        return numberofArtistsInserted;
      }


      public int insertSong(Song[] songs){
        int result = 0;
        int numberofSongsInserted = 0;

        List<String> insertSongs = new ArrayList<String>();


        for(int i = 0 ; i < songs.length ; i++){
          String query = "insert into song values (" +
                          songs[i].getSongID()+ ",\"" +
                          songs[i].getSongName() + "\",\"" +
                          songs[i].getGenre() + "\"," +
                          songs[i].getRating() + "," +
                          songs[i].getArtistID() + "," +
                          songs[i].getAlbumID() + ")";

         insertSongs.add(i,query);
        }


        for(int i = 0 ; i < insertSongs.size() ; i++){

          try {
              Statement st = con.createStatement();
              result = st.executeUpdate(insertSongs.get(i));
              numberofSongsInserted++;

              //Close
              st.close();

          } catch (SQLException e) {
              //e.printStackTrace();
              if (e.toString().contains("SQLIntegrityConstraintViolationException")){
                  //throw new AlreadyInsertedException();
              }
            }
        }
        return numberofSongsInserted;
      }

      public int insertUser(User[] users){
        int result = 0;
        int numberofUsersInserted = 0;

        List<String> insertUsers = new ArrayList<String>();


        for(int i = 0 ; i < users.length ; i++){
          String query = "insert into user values (" +
                          users[i].getUserID()+ ",\"" +
                          users[i].getUserName() + "\",\"" +
                          users[i].getEmail() + "\"," +
                          users[i].getPassword() + ")";

         insertUsers.add(i,query);
        }


        for(int i = 0 ; i < insertUsers.size() ; i++){

          try {
              Statement st = con.createStatement();
              result = st.executeUpdate(insertUsers.get(i));
              numberofUsersInserted++;

              //Close
              st.close();

          } catch (SQLException e) {
              //e.printStackTrace();
              if (e.toString().contains("SQLIntegrityConstraintViolationException")){
                  //throw new AlreadyInsertedException();
              }
            }
        }
        return numberofUsersInserted;        
      }


      
      public int insertListen(Listen[] listens){
        int result = 0;
        int numberofListensInserted = 0;

        List<String> insertListens = new ArrayList<String>();


        for(int i = 0 ; i < listens.length ; i++){
          String query = "insert into listen values (" +
                          listens[i].getUserID()+ "," +
                          listens[i].getSongID() + ",\"" +
                          listens[i].getLastListenTime() + "\"," +
                          listens[i].getListenCount() + ")";

         insertListens.add(i,query);
        }


        for(int i = 0 ; i < insertListens.size() ; i++){

          try {
              Statement st = con.createStatement();
              result = st.executeUpdate(insertListens.get(i));
              numberofListensInserted++;

              //Close
              st.close();

          } catch (SQLException e) {
              //e.printStackTrace();
              if (e.toString().contains("SQLIntegrityConstraintViolationException")){
                  //throw new AlreadyInsertedException();
              }
            }
        }
        return numberofListensInserted;   
      }


      public QueryResult.ArtistNameSongNameGenreRatingResult[] getHighestRatedSongs(){

          String query = "select distinct a.artistName, s.songName, s.genre, s.rating from artist a,"+
                         " song s where s.artistID = a.artistID and s.rating = (select MAX(s1.rating) from song s1) order by a.artistName ASC;";

          List<QueryResult.ArtistNameSongNameGenreRatingResult> q_list = new ArrayList<QueryResult.ArtistNameSongNameGenreRatingResult>();
         
          try{
             Statement statement = con.createStatement();

             int i = 0;

             ResultSet rs = statement.executeQuery(query);
             
             while(rs.next()){

              String a_name = rs.getString("a.artistName");
              String s_name = rs.getString("s.songName");
              String genre  = rs.getString("s.genre");
              Double    rating = rs.getDouble("s.rating");

              
              QueryResult.ArtistNameSongNameGenreRatingResult temp = new QueryResult.ArtistNameSongNameGenreRatingResult(a_name,s_name,genre,rating);
              q_list.add(i,temp) ;

              i++;

              
             }
             //close
             statement.close();
         }
         catch (SQLException e) {
             e.printStackTrace();
         }
        
          
         int size = q_list.size();
         QueryResult.ArtistNameSongNameGenreRatingResult[] queryResult = new QueryResult.ArtistNameSongNameGenreRatingResult[size];
         
         
         for(int j = 0 ; j<size ; j++){
             queryResult[j] = q_list.get(j);
         }
         
         return queryResult;
      }


      public QueryResult.TitleReleaseDateRatingResult getMostRecentAlbum(String artistName){
        
        String query = "select al.title, al.releaseDate, al.albumRating from album al where al.albumID = " +
        "(select al1.albumID from album al1, artist a where"+ " a.artistname = '"+ artistName + "' and a.artistID = al1.artistID "+
        "and al1.releaseDate = ( select MAX(al2.releaseDate)"+ " from album al2 where al2.artistID = a.artistID)) order by al.albumRating DESC;";

        QueryResult.TitleReleaseDateRatingResult queryResult = null;


        try{
              Statement statement = this.con.createStatement();

              ResultSet rs = statement.executeQuery(query);
              if(rs.next()){
                String a_title = rs.getString("al.title");
                String s_releaseDate = rs.getString("al.releaseDate");
                Double    rating = rs.getDouble("al.albumRating");


                queryResult = new QueryResult.TitleReleaseDateRatingResult(a_title,s_releaseDate,rating);
              }
             //close
             statement.close();
         }
         catch (SQLException e) {
             e.printStackTrace();
         }


        return queryResult;

      }


      public QueryResult.ArtistNameSongNameGenreRatingResult[] getCommonSongs(String userName1, String userName2){
        
        List<QueryResult.ArtistNameSongNameGenreRatingResult> q_list = new ArrayList<QueryResult.ArtistNameSongNameGenreRatingResult>();
        
        String query = "select distinct a.artistName, s.songname, s.genre, s.rating "+
                        "from song s, listen l1, listen l2, user u1, user u2, artist a "+
                        "where u1.userName = '" + userName1 + "' and u2.username = '" + userName2 + 
                        "' and u1.userID = l1.userID and u2.userID = l2.userID"+
                        " and l1.songID = s.songID and l2.songID = s.songID and s.artistID = a.artistID order by s.rating DESC;";

         try{
             Statement statement = this.con.createStatement();

             int i = 0;

             ResultSet rs = statement.executeQuery(query);
             
             while(rs.next()){

              String s_name = rs.getString("a.artistName");
              String s_sname = rs.getString("s.songName");
              String genre  = rs.getString("s.genre");
              int    rating = rs.getInt("s.rating");

             QueryResult.ArtistNameSongNameGenreRatingResult temp = new QueryResult.ArtistNameSongNameGenreRatingResult(s_name,s_sname,genre,rating);
              q_list.add(i,temp); 

              i++;
              
             }
             //close
             statement.close();
         }
         catch (SQLException e) {
             e.printStackTrace();
         }

         int size = q_list.size();
         
         QueryResult.ArtistNameSongNameGenreRatingResult[] queryResult = new QueryResult.ArtistNameSongNameGenreRatingResult[size];  
         
         for(int j = 0 ; j <size ; j++){
             queryResult[j] = q_list.get(j);
         }
         
        return queryResult;
      }


      public QueryResult.ArtistNameNumberOfSongsResult[] getNumberOfTimesSongsListenedByUser(String userName){
        
        List<QueryResult.ArtistNameNumberOfSongsResult> q_list = new ArrayList<QueryResult.ArtistNameNumberOfSongsResult>();
        
        String query = "select a.artistName, sum(l.listenCount) as lcount from song s, user u, listen l, artist a where u.userName = '" + userName + "' and u.userID = l.userID "
                       +" and l.songID = s.songID and s.artistID = a.artistID group by a.artistID order by a.artistName ASC ;";


         try{
             Statement statement = this.con.createStatement();

             int i = 0;

             ResultSet rs = statement.executeQuery(query);
             
             while(rs.next()){

              String a_name = rs.getString("a.artistName");
              int lcount = rs.getInt("lcount");

              QueryResult.ArtistNameNumberOfSongsResult temp = new QueryResult.ArtistNameNumberOfSongsResult(a_name,lcount);
              q_list.add(i,temp);
                      
              i++;
              
             }
             //close
             statement.close();
         }
         catch (SQLException e) {
             e.printStackTrace();
         }
         
        int size = q_list.size();
        QueryResult.ArtistNameNumberOfSongsResult[] queryResult = new QueryResult.ArtistNameNumberOfSongsResult[size];
        
        for(int j=0;j<size;j++){
            queryResult[j] = q_list.get(j);
        }
        return queryResult;

      }

      
      public User[] getUsersWhoListenedAllSongs(String artistName){
        
        List<User> q_list = new ArrayList<User>();
        
        String query = " select u.userID,u.userName,u.email,u.password from user u where not exists (" +
                       " Select s.songID from song s,artist a where s.artistID = a.artistID"+
                       " and a.artistName = '"+ artistName + "'  and not exists  (" +
                       " select l.songID from listen l where l.songID = s.songID and l.userID = u.userID)) order by u.userID ASC;";
        
        
        try{
             Statement statement = this.con.createStatement();

             int i = 0;

             ResultSet rs = statement.executeQuery(query);
             
             while(rs.next()){

              int u_ID = rs.getInt("u.userID");
              String u_name = rs.getString("u.userName");
              String u_email = rs.getString("u.email");
              String u_password = rs.getString("u.password");

              User temp = new User(u_ID,u_name,u_email,u_password);
              q_list.add(i,temp);
                      
              i++;
              
             }
             //close
             statement.close();
         }
         catch (SQLException e) {
             e.printStackTrace();
         }
         
        int size = q_list.size();
        User[] queryResult = new User[size];
        
        for(int j=0;j<size;j++){
            queryResult[j] = q_list.get(j);
        }
        return queryResult;

      }

      
      public QueryResult.UserIdUserNameNumberOfSongsResult[]  getUserIDUserNameNumberOfSongsNotListenedByAnyone(){
        List<QueryResult.UserIdUserNameNumberOfSongsResult> q_list = new ArrayList<QueryResult.UserIdUserNameNumberOfSongsResult>();
        
        String query = "select u.userID,u.userName, COUNT(*) as lcount from user u, listen l where u.userID = l.userID and l.songID " +
                       " NOT IN ( select l1.songID from listen l1, user u1 where l1.userID = u1.userID and u1.userID <> u.userID)"+
                       " group by u.userID order by u.userID ASC;";
       
        
        try{
             Statement statement = this.con.createStatement();

             int i = 0;

             ResultSet rs = statement.executeQuery(query);
             
             while(rs.next()){

              int u_ID = rs.getInt("u.userID");
              String u_name = rs.getString("u.userName");
              int lcount = rs.getInt("lcount");

              QueryResult.UserIdUserNameNumberOfSongsResult temp = new QueryResult.UserIdUserNameNumberOfSongsResult(u_ID,u_name,lcount);
              q_list.add(i,temp);
                      
              i++;
              
             }
             //close
             statement.close();
         }
         catch (SQLException e) {
             e.printStackTrace();
         }
         
        int size = q_list.size();
        QueryResult.UserIdUserNameNumberOfSongsResult[] queryResult = new QueryResult.UserIdUserNameNumberOfSongsResult[size];
        
        for(int j=0;j<size;j++){
            queryResult[j] = q_list.get(j);
        }
        return queryResult;
      }


      public Artist[] getArtistSingingPopGreaterAverageRating(double rating){
        
        List<Artist> q_list = new ArrayList<Artist>();
        
        String query = "select a.artistID,a.artistName,AVG(s.rating) from artist a, song s"+
                " where s.artistID = a.artistID and a.artistID "+
                " IN (select distinct a.artistID from artist a, song s where s.artistID = a.artistID and s.genre = \"Pop\")"+
                " group by s.artistID having AVG(s.rating) >" + rating + " order by s.artistID DESC;";
        
        
        try{
             Statement statement = this.con.createStatement();

             int i = 0;

             ResultSet rs = statement.executeQuery(query);
             
             while(rs.next()){

              int aID = rs.getInt("a.artistID");
              String a_name = rs.getString("a.artistName");

              Artist temp = new Artist(aID,a_name);

              q_list.add(i,temp);
                      
              i++;
              
             }
             //close
             statement.close();
         }
         catch (SQLException e) {
             e.printStackTrace();
         }
         
        int size = q_list.size();
        Artist[] queryResult = new Artist[size];
        
        for(int j=0;j<size;j++){
            queryResult[j] = q_list.get(j);
        }
        return queryResult;
        
      }

             
       //query do not return any result, check!!
      public Song[] retrieveLowestRatedAndLeastNumberOfListenedSongs(){
        
        List<Song> q_list = new ArrayList<Song>();
        
        String query = "select s.songID,s.songName,s.rating,s.genre,s.artistID,s.albumID,COUNT(*) as lcount"+
                       " from listen l, song s where s.songID = l.songID and l.songID " +
                       "IN (select s.songID from song s where s.rating IN (select MIN(s1.rating)"+ 
                       " from song s1 where s1.genre = ? ))" +
                       " group by l.songID having MIN(lcount) order by s.songID ASC;";
        
        String pop = "Pop";
        
        try{
             PreparedStatement statement = this.con.prepareStatement(query);
             
             statement.setString(1, pop);

             int i = 0;

             ResultSet rs = statement.executeQuery();
             //query do not return any result, check!!
             
             while(rs.next()){

              int sID = rs.getInt("s.songID");
              String s_name = rs.getString("s.songName");
              Double s_rating = rs.getDouble("s.rating");
              String genre = rs.getString("s.genre");
              int artist_ID = rs.getInt("s.artistID");
              int album_ID = rs.getInt("s.albumID");

              Song temp = new Song(sID,s_name,genre,s_rating,artist_ID,album_ID);

              q_list.add(i,temp);
                      
              i++;
              
             }
             //close
             statement.close();
         }
         catch (SQLException e) {
             e.printStackTrace();
         }
         
        int size = q_list.size();
        Song[] queryResult = new Song[size];
        
        for(int j=0;j<size;j++){
            queryResult[j] = q_list.get(j);
        }
        return queryResult;
        
      }

      
      public int multiplyRatingOfAlbum(String releaseDate){
        
        String sql_safe_updates = "SET SQL_SAFE_UPDATES = 0";
        String query_update = " update album set album.albumRating = (1.5)*album.albumRating where album.releaseDate > '"
                        +releaseDate+"';";
        
        
        int queryResult = 0;

        try{
              
              PreparedStatement ps_safe_updates=this.con.prepareStatement(sql_safe_updates);
              ps_safe_updates.execute();
              
              PreparedStatement p_statement = this.con.prepareStatement(query_update);
              queryResult = p_statement.executeUpdate(query_update);


             //close
             ps_safe_updates.close();
             p_statement.close();
         }
         catch (SQLException e) {
             e.printStackTrace();
         }

        return queryResult;
      }


      public Song deleteSong(String songName){
        Song result = null;
        String query = "select * from song s where s.songName = '" + 
                                songName + "';";
        String query_delete = "delete from song where song.songName = \""+ songName + "\" ;";

        try{
            Statement statement = this.con.createStatement();

            ResultSet rs = statement.executeQuery(query);
            if(rs.next()){
                int songID = rs.getInt("s.songID");
                String sName = rs.getString("s.songName");
                String    genre = rs.getString("s.genre");
                Double   rating = rs.getDouble("s.rating");
                int    artistID = rs.getInt("s.artistID");
                int    albumID  = rs.getInt("s.albumID"); 

             
                result = new Song(songID,sName,genre,rating,artistID,albumID);
            }
           //close
           statement.close();
        }
        catch (SQLException e) {
             e.printStackTrace();
        }

        try{
          Statement statement = this.con.createStatement();
          int rs;
          rs = statement.executeUpdate(query_delete);
 
          statement.close();
         }
         catch (SQLException e) {
             e.printStackTrace();
         }

        return result;
      }

}
