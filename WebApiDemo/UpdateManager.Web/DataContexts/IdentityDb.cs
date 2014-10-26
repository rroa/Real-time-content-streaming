using Microsoft.AspNet.Identity.EntityFramework;
using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Web;
using UpdateManager.Web.Models;

namespace UpdateManager.Web.DataContexts
{
    public class IdentityDb : IdentityDbContext<ApplicationUser>
    {
        public IdentityDb()
            : base("DefaultConnection")
        { }        

        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            modelBuilder.HasDefaultSchema("Identity");
            base.OnModelCreating(modelBuilder);
        }
    }
}