#include <stdio.h>
#include <fcntl.h>
/*
 * @Author: Hannes de Waal (c) 2006
 *
 * @Description:
 * Logger module
 *
 * @log:
 * 18 May 2004  - version 1.0
 *
 *
 */

char *
parse_profile (char *file, char *find_param)
{
  FILE *in;
  char line[200];
  char *delimiters = "\n";
  char *token = NULL;
  char *sp = NULL;
  char *spt = NULL;
  char *param = NULL;
  char *value = NULL;
  char val[200] = "";
  int found = 0;

  if (file == NULL)
    {
      loge ("no profile specified %s.", file);
      return NULL;
    }
  if ((in = fopen (file, "r")) == NULL)
    {
      loge ("Can't open profile [%s]", file);
      return NULL;
    }
  /* Read each line one at a time */
  while (!feof (in))
    {
      /* Get one line */
      fgets (line, 80, in);
      if (!feof (in))
	{
	  /* Break the line up into words */
	  token = strtok (line, delimiters);
	  while (token != NULL && found == 0)
	    {
//                puts(token);
	      if (token[0] != '#')
		{		/* no comment */
		  spt = sp = (char *) strstr (token, "=");
		  if (sp != NULL)
		    {		/*  delimiter */
		      sp[0] = '\0';
		      param = token;
		      while (*token != '\0')
			{
			  token++;
			  if (isspace (*token))
			    *token = '\0';
			}

		      //trim_trailing_ws(token,strlen(token));
		      value = ++sp;
		      while (isspace (*value))
			value++;
		      if (strstr (find_param, param) != NULL)
			{
			  strcpy ((char *) &val, value);
			  found++;
			}

		      //printf("param >%s<<>>%s<\n",param,value);
		    }
		}
	      /* Get the next word */
	      token = strtok (NULL, delimiters);
	    }
	}
    }
  fclose (in);
  sp = NULL;
  sp = (char *) &val;
  if (found != 1)
    {
      logd ("Can't find parameter: %s in [%s]", find_param, file);
    }
  return (found == 1) ? sp : NULL;
}

/*
* demonize and lockfile ops start here
*/
int
daemonize (const char *lockfile)
{
  pid_t pid;
  int fd = 0;

  if (lockfile)
    {

      lockfile = get_absolute_filename (lockfile);
      if (!lockfile)
	return -1;

      fd = lockfile_get_exclusive (lockfile);
      if (fd < 0)
	return -1;

    }

  pid = fork ();
  if (pid < 0)
    {
      loge ("fork failed.");
      return -1;
    }

  else if (pid)
    {
      if (lockfile)
	{

	  lockfile_write_pid (fd, pid);

	}


      logi ("Starting as daemon: PID is %d.", (int) pid);
      _exit (0);
    }

  setsid ();
  //chdir("/");
  umask (022);

  fclose (stdin);
  fclose (stdout);
  fclose (stderr);

  /*
   * We want the lock to be unlinked upon normal exit.
   if ( lockfile ) 
   atexit(lockfile_unlink);
   */

  return 0;
}

static int
lockfile_write_pid (int fd, pid_t pid)
{
  int ret;
  char buf[50];

  /*
   * Reset file size to 0.
   */
  ret = ftruncate (fd, 0);
  if (ret < 0)
    {
      fprintf (stderr, "couldn't truncate lockfile to 0 byte.");
      return -1;
    }

  snprintf (buf, sizeof (buf), "%d\n", (int) pid);

  ret = write (fd, buf, strlen (buf));
  if (ret < 0)
    {
      fprintf (stderr, "couldn't write PID to lockfile.");
      return -1;
    }

  return 0;
}


int
lockfile_unlink (void)
{
  int ret;

  ret = unlink (slockfile);
  if (ret < 0)
    loge ("couldn't delete %s.", slockfile);
}




static const char *
get_absolute_filename (const char *lockfile)
{
  if (*lockfile == '/')
    snprintf (slockfile, sizeof (slockfile), "%s", lockfile);

  else
    {
      char dir[MAXLINE];

      /*
       * if lockfile is a relative path,
       * deletion on exit() will not work because of the chdir("/") call.
       * That's why we want to conver it to an absolute path.
       */
      if (!getcwd (dir, sizeof (dir)))
	{
	  loge ("couldn't get current working directory.");
	  return NULL;
	}

      snprintf (slockfile, sizeof (slockfile), "%s/%s", dir, lockfile);
    }

  return slockfile;
}

static int
lockfile_get_exclusive (const char *lockfile)
{
  int ret, fd;
  struct flock lock;

  fd = open (lockfile, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (fd < 0)
    {
      fprintf (stderr, "couldn't open %s for writing.\n", lockfile);
      return -1;
    }

  lock.l_type = F_WRLCK;	/* write lock */
  lock.l_start = 0;		/* from offset 0 */
  lock.l_whence = SEEK_SET;	/* at the beggining of the file */
  lock.l_len = 0;		/* until EOF */
  lock.l_pid = getpid ();	/* our PID                      */


  ret = fcntl (fd, F_SETLK, &lock);
  if (ret == -1)
    {
      if (errno == EACCES || errno == EAGAIN)
	fprintf (stderr, "program is already running.\n");
      // logi("program is already running.");
      else
	fprintf (stderr, "couldn't set write lock.\n");

      close (fd);
      return -1;
    }
//      fprintf(stdout,"got lock");

  /*
   * lock is now held until program exit.
   */
  return fd;
}


/*
int main(char *argc, char **argv)
{
int i;
char p = { ":"};
char buff[256];
    //if (argv[1] == NULL) exit(0);
    //parse(argv[1]);
char *test_proj[] = {  "loptT00D00" , "loptT00DB"  ,"loptT00DVEBMGS00" };
    for (i=0;i<3;i++){
       get_lhost(test_proj[i],p,&buff);
    }

}
*/
