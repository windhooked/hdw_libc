
static char slockfile[256];

/*
#define REALLOC_INCREMENT  10
static void
realloc_arrays (int nsize)
{
    nsize += REALLOC_INCREMENT;
    if (nsize <= process_array_size)
        return;

    process_array_size = nsize;
    process_array = (proc_info_t *)realloc(process_array,
                                           (nsize*sizeof(proc_info_t)));
    old_process_array = (old_proc_info_t *)realloc(old_process_array,
                        (nsize*sizeof(old_proc_info_t)));

    if (!process_array || !old_process_array) {
        logf ("%s: can't allocate memory", myname);
        //fprintf (stderr,"%s: can't allocate memory\n", myname);
        quit(1);
    }
}
*/
/*
 *
 */
char *
get_profile (int type, char *sid, char *iid)
{
  char profile_dir[100];
  char name[100];
  int err = 0;
  int found = 0;
  char *p = NULL;

  //profile[160] = '\0';

  //if (type == 1)
  {
    sprintf (profile_dir, "/sapmnt/%s/profile/", sid);
    //   sprintf (profile_dir, "/sapmnt/%s/SYS/profile/", sid);
  }

  {
    DIR *dir = opendir (profile_dir);
    struct dirent *ent;
    if (dir == NULL)
      {
	loge ("Can't open dir [%s]", profile_dir);
	return NULL;
      }

    while (found == 0 && (ent = readdir (dir)) != NULL)
      {

//            printf("%s\n",ent->d_name);
	if (ent->d_name[0] == '.')
	  continue;
	if (strstr (ent->d_name, "bak") != NULL)
	  continue;

	switch (type)
	  {
	  case 0:
	    sprintf (name, "%s_DVEBMGS%s_", sid, iid);
	    logd ("%s =? %s", name, ent->d_name);
	    if (strstr (ent->d_name, name) == NULL)
	      continue;
	    logd ("%s == %s", name, ent->d_name);
	    strcpy (name, ent->d_name);
	    found++;		/* just to check if there are more than one profile */
	    break;
	  case 1:
	    logd ("%s =? %s", "DEFAULT.PFL", ent->d_name);
	    if (strcmp ("DEFAULT.PFL", ent->d_name))
	      continue;
	    logd ("%s == %s", "DEFAULT.PFL", ent->d_name);
	    strcpy (name, ent->d_name);
	    found = 1;
	    break;
	  case 2:
	    sprintf (name, "%s_D%s_", sid, iid);
	    logd ("%s =? %s", name, ent->d_name);
	    if (strstr (ent->d_name, name) == NULL)
	      continue;
	    logd ("%s == %s", name, ent->d_name);
	    strcpy (name, ent->d_name);
	    found++;		/* just to check if there are more than one profile */
	    break;
	  default:
	    err++;
	    break;
	  }
      }
    closedir (dir);
  }
  logd ("Found %d, [%s]", found, name);

  if (found == 1)
    {
      p = (char *) &profile;
      /* whats wrong with strcpyr,strcat ?? */
      sprintf (p, "%s%s", profile_dir, name);
      return p;
    }
  else
    {
      loge ("Found more than one profile at [%s]", profile_dir);
      return NULL;
    }
}

/* 
 *
 */
char *
get_lhost (char *proj_name, char *pf, char *buffer)
{
  char instance[8];
  char sid[4];
  char iid[3];
  char pfl[200];
  char *psid, *piid, *pinstance;
  char *profile = NULL;
  char *lhost = NULL;
  char type = 0;
  char *types[] = { "Central Instance", "DB Instance", "Application Server" };
  char *tp;
  int err = 0;
  char proj[40];

  int i;
  if (proj_name == NULL && pf == NULL)
    return hostname;
  strcpy (proj, proj_name);

//  fprintf(stderr,"get logical host for %s", proj);
  logd ("get logical host for %s", proj);
  /* look if the proc iteration found active profiles, only last one found will be taken */
  /* this poses a problem if more than one system are part of a project */
  if (strstr (pf, "pf=/usr/sap/") != NULL)
    {
      /* yes */
      tp = strtok (pf, "=");
      tp = strtok (NULL, "=");
      /* get pf= and parse profile for SAPLOCALHOST */
      lhost = (char *) parse_profile (tp, "SAPLOCALHOST");
    }
  else
    {
      /* no */
      /* search for profiles */
      /* lets try dismanteling the project name for our search of profiles */

      type = parse_proj_name (&proj, &sid, &instance, &iid);

      if (type >= 0 && type <= 2)
	{

	  logd ("sid: %s instance: %s iid: %s.", sid, instance, iid);
	  //logd("Found %s ,type [%d].",types[type] ,type); 
	  profile = get_profile (type, (char *) &sid, (char *) &iid);

	  if (profile != NULL)
	    {
	      logd ("Profile: %s", profile);
	      strcpy (pfl, profile);
	      lhost =
		(char *) parse_profile (&pfl,
					((type ==
					  1) ? "SAPDBHOST" : "SAPLOCALHOST"));
	    }
	  else
	    {
	      logd
		("Can't determine profile with sid: %s iid: %s instance: %s error: %d",
		 sid, iid, instance, type);
	    }
	}
      else
	{
	  logd ("Can't determine type for project: %s error: %d", proj, type);
	}
//        printf("lhost: %s\n",lhost);
    }
  if (lhost == NULL)
    {
      loge ("Failed to determine logical host name for project. [%s][%d]",
	    proj, type);
    }
  else
    {
      //trim_space(lhost); 
      //int i = strlen(lhost); lhost[i+1] = '\0'; 
      strcpy(buffer,lhost);
      logd ("lhost: %s", lhost);
      logd ("buffer lhost: %s", buffer);
    }
  //return (lhost == NULL) ? hostname : lhost;
  return buffer;

}

/*
 * return type or negative error value
 */
int
parse_proj_name (char *proj_name, char *sid, char *instance, char *iid)
{
  int l = 0;
  int err = 0;
  int type = 0;
  char d[10];
  char *p = NULL;
  p = proj_name;

  l = strlen (proj_name);
  logd ("strlen(%s) => %d", proj_name, l);
  switch (l)
    {
      /* loptP01DB */
    case 9:
      strnxcpy (sid, p, 4, 3);
      strnxcpy (instance, p, 7, 2);
      iid[0] = '\0';
      type = 1;
      break;
      /* loptP01D01 */
    case 10:
      strnxcpy (sid, p, 4, 3);
      strnxcpy (instance, p, 7, 1);
      strnxcpy (iid, p, 8, 2);
      type = 2;
      break;
      /* loptP01DVEBMGS01 */
    case 16:
      strnxcpy (sid, p, 4, 3);
      strnxcpy (instance, p, 7, 7);
      strnxcpy (iid, p, 14, 2);
      type = 0;
      break;
    default:
      err--;
      break;
    }
  if (err < 0)
    {
      logd ("Failed to parse project name for sid,iid,instance name. [%s]",
	    proj_name);
    }
  return (err < 0) ? err : type;
}

