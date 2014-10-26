using System.Linq;
using System.Web.Http;
using UpdateManager.Web.DataContexts;
using UpdateManager.Web.Helpers;

namespace UpdateManager.Web.Controllers
{
    public class Config
    {
        //public int UpdateId { get; set; }

        //public string UpdateName { get; set; }

        public string Version { get; set; }
    }

    public class UpdateManagerController : ApiController
    {
        private readonly UpdatesDb _db = new UpdatesDb();
        public IFileProvider FileProvider { get; set; }
        public UpdateManagerController()
        {
            FileProvider = new FileProvider();
        }

        // GET api/<controller>
        [Route("api/application/version")]
        public Config GetApplicationVersion()
        {
            var appVersion = _db.ApplicationVersion.OrderByDescending(app => app.Id).FirstOrDefault();
            string currentVersion = appVersion != null ?
                string.Format("{0}.{1}.{2}", appVersion.Major, appVersion.Minor, appVersion.Patch)
                : "0.0.0";
            //var p = new Config { Version = currentVersion, UpdateId = appVersion.UpdateId, UpdateName = appVersion.ApplicationUpdate.Name };
            var p = new Config { Version = currentVersion };
            return p;
        }      

        // POST api/<controller>
        public void Post([FromBody]string value)
        {
        }

        // PUT api/<controller>/5
        public void Put(int id, [FromBody]string value)
        {
        }

        // DELETE api/<controller>/5
        public void Delete(int id)
        {
        }
    }
}