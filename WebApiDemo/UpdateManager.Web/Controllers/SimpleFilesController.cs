using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Web.Http;
using System.Xml;
using UpdateManager.Entities;
using UpdateManager.Web.DataContexts;
using UpdateManager.Web.Helpers;

namespace UpdateManager.Web.Controllers
{
    public class ManifestFile
    {        
        public string Manifest { get; set; }
        
    }
    public class SimpleFilesController : ApiController
    {
        private readonly UpdatesDb _db = new UpdatesDb();
        public IFileProvider FileProvider { get; set; }

        public SimpleFilesController()
        {
            FileProvider = new FileProvider();
        }

        public HttpResponseMessage Get(string fileName)
        {
            if (!FileProvider.Exists(fileName))
            {
                throw new HttpResponseException(HttpStatusCode.NotFound);
            }

            FileStream fileStream = FileProvider.Open(fileName);
            var response = new HttpResponseMessage { Content = new StreamContent(fileStream) };
            response.Content.Headers.ContentDisposition
                = new ContentDispositionHeaderValue("attachment") { FileName = fileName };

            response.Content.Headers.ContentType
                = new MediaTypeHeaderValue("application/octet-stream");
            response.Content.Headers.ContentLength
                    = FileProvider.GetLength(fileName);
            return response;
        }

        [Route("api/application/manifest/{id}")]
        public ManifestFile GetManifest(string id)
        {
            string[] roles = id.Split(',');
            var files = new List<PackageFile>();
            var appVersion = _db.ApplicationVersion.OrderByDescending(app => app.Id).FirstOrDefault();
            var latestUpdate = _db.Updates.FirstOrDefault(update => update.Id == appVersion.UpdateId);

            foreach (var role in roles)
            {
                string currentRole = role;
                var roleFiles = _db.PackageFiles.Where(file => file.Roles.Contains(currentRole)
                    && file.UpdatePackage.UpdateId == appVersion.UpdateId);

                foreach (var file in roleFiles)
                {
                    if (!files.Contains(file))
                    {
                        files.Add(file);
                    }
                }
            }

            id = id.Replace(",", "");
            string fileName = string.Format("manifest-{0}-{1}.xml", appVersion.UpdateId, id);
            if (files.Count > 0)
            {
                string manifestFileName = string.Format(@"C:\Users\c20demo\Documents\visual studio 2013\Projects\WebApiDemo\UpdateManager.Web\Files\{0}", fileName);
                using (XmlWriter writer = XmlWriter.Create(manifestFileName))
                {
                    writer.WriteStartDocument();
                    writer.WriteStartElement("manifest");

                    // applicationIdentity
                    writer.WriteStartElement("applicationIdentity");
                    writer.WriteAttributeString("version", string.Format("{0}.{1}.{2}", appVersion.Major, appVersion.Minor, appVersion.Patch));
                    writer.WriteAttributeString("name", "RPG Demo");
                    writer.WriteAttributeString("author", "Raul Roa");
                    writer.WriteEndElement();
                    
                    writer.WriteElementString("description",latestUpdate.Description);
                    
                    // Packages
                    writer.WriteStartElement("packages");

                    foreach (var group in files.OrderBy(o => o.UpdatePackage.Priority).GroupBy(p => p.UpdatePackage.Priority))
                    {
                        writer.WriteStartElement("package");
                        writer.WriteAttributeString("priority", group.Key.ToString());
                        foreach (var updateFile in group)
                        {
                            writer.WriteStartElement("file");
                            writer.WriteAttributeString("id", updateFile.Id.ToString(CultureInfo.InvariantCulture));
                            writer.WriteAttributeString("jobType","Download");
                            writer.WriteAttributeString("file", updateFile.FileName);
                            writer.WriteEndElement();
                        }
                        writer.WriteEndElement();         
                    }

                    writer.WriteEndElement();

                    writer.WriteEndElement();
                    writer.WriteEndDocument();                    
                }
            }

            return new ManifestFile { Manifest = fileName };
        }
	}
}