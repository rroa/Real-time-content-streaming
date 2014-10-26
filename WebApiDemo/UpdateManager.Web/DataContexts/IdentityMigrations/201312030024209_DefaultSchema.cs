namespace UpdateManager.Web.DataContexts.IdentityMigrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class DefaultSchema : DbMigration
    {
        public override void Up()
        {
            MoveTable(name: "dbo.AspNetRoles", newSchema: "Identity");
            MoveTable(name: "dbo.AspNetUserClaims", newSchema: "Identity");
            MoveTable(name: "dbo.AspNetUsers", newSchema: "Identity");
            MoveTable(name: "dbo.AspNetUserLogins", newSchema: "Identity");
            MoveTable(name: "dbo.AspNetUserRoles", newSchema: "Identity");
        }
        
        public override void Down()
        {
            MoveTable(name: "Identity.AspNetUserRoles", newSchema: "dbo");
            MoveTable(name: "Identity.AspNetUserLogins", newSchema: "dbo");
            MoveTable(name: "Identity.AspNetUsers", newSchema: "dbo");
            MoveTable(name: "Identity.AspNetUserClaims", newSchema: "dbo");
            MoveTable(name: "Identity.AspNetRoles", newSchema: "dbo");
        }
    }
}
