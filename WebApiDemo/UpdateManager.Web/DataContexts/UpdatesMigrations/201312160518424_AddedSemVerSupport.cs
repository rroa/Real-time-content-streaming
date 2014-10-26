namespace UpdateManager.Web.DataContexts.UpdatesMigrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddedSemVerSupport : DbMigration
    {
        public override void Up()
        {
            AddColumn("UpdateManager.ApplicationVersions", "Major", c => c.Int(nullable: false));
            AddColumn("UpdateManager.ApplicationVersions", "Minor", c => c.Int(nullable: false));
            AddColumn("UpdateManager.ApplicationVersions", "Patch", c => c.Int(nullable: false));
        }
        
        public override void Down()
        {
            DropColumn("UpdateManager.ApplicationVersions", "Patch");
            DropColumn("UpdateManager.ApplicationVersions", "Minor");
            DropColumn("UpdateManager.ApplicationVersions", "Major");
        }
    }
}
