let mainNav = document.getElementById('js-menu');
let navBarToggle = document.getElementById('js-nav-toggle');

navBarToggle.addEventListener("click", function(){
  mainNav.classList.toggle('activeInfo');
});

const observer = new IntersectionObserver((entries) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      const el = entry.target;
      el.classList.add("active");

      el.addEventListener("mouseenter", () => {
        el.style.transitionDelay = "0s";
      });
      el.addEventListener("mouseleave", () => {
        el.style.transitionDelay = "0s";
      });

      observer.unobserve(el);
    }
  });
}, { threshold: 0.2 });

document.querySelectorAll(".reveal").forEach(el => observer.observe(el));
