using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Data.Entity.Migrations;
using System.IO;
using System.Linq;
using System.Net;
using System.Web;
using System.Web.Mvc;
using Newtonsoft.Json;
using UpdateManager.Entities;
using UpdateManager.Web.DataContexts;
using UpdateManager.Web.ViewModels;

namespace UpdateManager.Web.Controllers
{
    [Authorize]
    public class UpdatesController : Controller
    {
        private readonly UpdatesDb _db = new UpdatesDb();

        // GET: /Updates/
        public ActionResult Index()
        {
            var appVersion = _db.ApplicationVersion.OrderByDescending(app => app.Id).FirstOrDefault();
            ViewBag.AppVersion = appVersion != null ? 
                string.Format("{0}.{1}.{2}", appVersion.Major, appVersion.Minor, appVersion.Patch) 
                : "0.0.0";                

            return View(_db.Updates.ToList());
        }

        // GET: /Updates/Details/5
        public ActionResult Details(int? id)
        {
            if (id == null)
            {
                return new HttpStatusCodeResult(HttpStatusCode.BadRequest);
            }
            Update update = _db.Updates.Find(id);
            if (update == null)
            {
                return HttpNotFound();
            }
            return View(update);
        }

        // GET: /Updates/Create
        public ActionResult Create()
        {
            var roles = new List<string> {"Halloween", "General"};

            var items = roles.Select(role => new SelectListItem
            {
                Text = role, Value = role
            }).ToList();

            ViewBag.Roles = items;

            return View();
        }

        // POST: /Updates/Create
        // To protect from overposting attacks, please enable the specific properties you want to bind to, for 
        // more details see http://go.microsoft.com/fwlink/?LinkId=317598.
        [HttpPost]
        [ValidateAntiForgeryToken]
        //public ActionResult Create([Bind(Include="Id,Name,Description,ChangeLogPath,Type,Status,CreationDate,PublishedDate,CreatedBy")] Update update)
        public ActionResult Create(UpdateViewModel viewModelToCreate)
        {
            var files = Request.Form["Files"];
            var updateFiles = new List<FileViewModel>();
            if (files != String.Empty)
            {
                updateFiles = JsonConvert.DeserializeObject<List<FileViewModel>>(files);
            }
            
            if (ModelState.IsValid)
            {
                string path = AppDomain.CurrentDomain.BaseDirectory + "Files/";
                string filePath = Path.Combine(path, viewModelToCreate.ChangeLog.FileName);
                viewModelToCreate.ChangeLog.SaveAs(filePath);

                var newUpdate = new Update
                {
                    Name = viewModelToCreate.Name,
                    ChangeLogPath = "~/Files/" + viewModelToCreate.ChangeLog.FileName,
                    CreatedBy = User.Identity.Name,
                    CreationDate = DateTime.Now,
                    PublishedDate = DateTime.Now,
                    Description = viewModelToCreate.Description,
                    Status = UpdateStatus.PUBLISHED,
                    Type = viewModelToCreate.Type
                };

                _db.Updates.AddOrUpdate(newUpdate);
                _db.SaveChanges();
                var packages = new List<UpdatePackage>();
                foreach (var file in updateFiles)
                {
                    FileViewModel fileViewModel = file;
                    var exists = packages.FirstOrDefault(p => p.Priority == fileViewModel.Type);
                    if (exists == null)
                    {
                        var newUpdatePkg = new UpdatePackage
                        {
                            UpdateId = newUpdate.Id,
                            CreatedBy = User.Identity.Name,
                            CreationDate = DateTime.Now,
                            UpdateDate = DateTime.Now,
                            Priority = fileViewModel.Type
                        };     

                        packages.Add(newUpdatePkg);
                    }
                }

                foreach (var pkg in packages)
                {
                    UpdatePackage updatePackage = pkg;
                    var pkgFiles = updateFiles.Where(f => f.Type == updatePackage.Priority);
                    var fileViewModels = pkgFiles as IList<FileViewModel> ?? pkgFiles.ToList();
                    if (fileViewModels.Any())
                    {
                        _db.Packages.Add(updatePackage);
                        _db.SaveChanges();
                        foreach (var pkgFile in fileViewModels)
                        {                                                        
                            var updatePackageFile = new PackageFile
                            {
                                FileName = pkgFile.Name,
                                FilePath = "~/Files/" + pkgFile.Name,
                                UpdatePackageId = updatePackage.Id,
                                CreationDate = DateTime.Now,
                                CreatedBy = User.Identity.Name,
                                FileSize = pkgFile.Size,
                                Roles = pkgFile.Roles
                            };

                            _db.PackageFiles.Add(updatePackageFile);
                            _db.SaveChanges();
                        }
                    }
                }

                var applicationVersion = _db.ApplicationVersion.OrderByDescending(p => p.Id).FirstOrDefault() ?? new ApplicationVersion();
                ApplicationVersion newAppVersion = applicationVersion;
                newAppVersion.CreationDate = DateTime.Now;
                newAppVersion.UpdateId = newUpdate.Id;
                switch (viewModelToCreate.Type)
                {
                    case UpdateType.MAJOR:
                        newAppVersion.Major++;
                        break;
                    case UpdateType.MINOR:
                        newAppVersion.Minor++;
                        break;
                    case UpdateType.PATCH:
                        newAppVersion.Patch++;
                        break;
                }

                _db.ApplicationVersion.AddOrUpdate(newAppVersion);
                _db.SaveChanges();
                return RedirectToAction("Index");
            }

            var roles = new List<string> { "Halloween", "General" };

            var items = roles.Select(role => new SelectListItem
            {
                Text = role,
                Value = role
            }).ToList();

            ViewBag.Roles = items;

            return View();
        }

        // GET: /Updates/Edit/5
        public ActionResult Edit(int? id)
        {
            if (id == null)
            {
                return new HttpStatusCodeResult(HttpStatusCode.BadRequest);
            }
            Update update = _db.Updates.Find(id);
            if (update == null)
            {
                return HttpNotFound();
            }
            return View(update);
        }

        // POST: /Updates/Edit/5
        // To protect from overposting attacks, please enable the specific properties you want to bind to, for 
        // more details see http://go.microsoft.com/fwlink/?LinkId=317598.
        [HttpPost]
        [ValidateAntiForgeryToken]
        public ActionResult Edit([Bind(Include="Id,Name,Description,ChangeLogPath,Type,Status,CreationDate,PublishedDate,CreatedBy")] Update update)
        {
            if (ModelState.IsValid)
            {
                _db.Entry(update).State = EntityState.Modified;
                _db.SaveChanges();
                return RedirectToAction("Index");
            }
            return View(update);
        }

        // GET: /Updates/Delete/5
        public ActionResult Delete(int? id)
        {
            if (id == null)
            {
                return new HttpStatusCodeResult(HttpStatusCode.BadRequest);
            }
            Update update = _db.Updates.Find(id);
            if (update == null)
            {
                return HttpNotFound();
            }
            return View(update);
        }

        // POST: /Updates/Delete/5
        [HttpPost, ActionName("Delete")]
        [ValidateAntiForgeryToken]
        public ActionResult DeleteConfirmed(int id)
        {
            Update update = _db.Updates.Find(id);
            _db.Updates.Remove(update);
            _db.SaveChanges();
            return RedirectToAction("Index");
        }

        [HttpPost]
        public ActionResult SaveUploadedFile()
        {
            foreach (string fileName in Request.Files)
            {
                HttpPostedFileBase file = Request.Files[fileName];
                string path = AppDomain.CurrentDomain.BaseDirectory + "Files/";
                if (file != null)
                {
                    string filePath = Path.Combine(path, file.FileName);
                    file.SaveAs(filePath);
                    return Json(new { Message = String.Format("File {0} saved", file.FileName) });   
                }                                              
            }

            return Json(new { Message = "Error in saving file" });
        }

        [HttpPost]
        public JsonResult Test(string name)
        {
            return Json(name);
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                _db.Dispose();
            }
            base.Dispose(disposing);
        }
    }
}
