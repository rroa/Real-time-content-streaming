using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Diagnostics;
using System.Linq;
using System.Web;
using UpdateManager.Entities;

namespace UpdateManager.Web.DataContexts
{
    public class UpdatesDb : DbContext
    {
        public UpdatesDb()
            : base("DefaultConnection")
        {
            Database.Log = sql => Debug.Write(sql);
        }

        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            modelBuilder.HasDefaultSchema("UpdateManager");
            base.OnModelCreating(modelBuilder);
        }

        #region Sets
        public DbSet<ApplicationVersion> ApplicationVersion { get; set; }
        public DbSet<Update> Updates { get; set; }
        public DbSet<UpdatePackage> Packages { get; set; }
        public DbSet<PackageFile> PackageFiles { get; set; }
        #endregion
    }
}